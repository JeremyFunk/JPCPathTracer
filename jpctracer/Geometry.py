import numpy as np
import ctypes as ct
import copy
from jpctracer.types import (
    triangles_t,
    spheres_t,
    instance_t,
    geometries_t,
    ctracer,
    JPC_SPHERE,
    JPC_TRIANGLE,
    )
import math
from jpctracer.Material import MaterialFactory

ctracer.bvhtree_free.argtypes = [ct.c_void_p]
ctracer.bvhtree_build_triangles.argtypes = [triangles_t]
ctracer.bvhtree_build_triangles.restype = ct.c_void_p

ctracer.bvhtree_build_spheres.argtypes = [spheres_t]
ctracer.bvhtree_build_spheres.restype = ct.c_void_p

ctracer.bvhtree_build_instances.argtypes = [ct.POINTER(geometries_t)]
ctracer.bvhtree_build_instances.restype = ct.c_void_p


class Instance:
    #type = "SPHERES", "TRIANGLES"
    def __init__(self,id,type, material_slots_count,
                 callback_on_trans_change,callback_on_delete):
        self._id = id
        self.materials = [None] * material_slots_count
        self._transformation = np.identity(4,dtype=np.float32)
        self._type = type
        self._callback_on_trans_change = callback_on_trans_change
        self._callback_on_delete = callback_on_delete

    def _get_transformation(self):
        return self._transformation

    def _set_transformation(self,value):
        self._transformation = value
        self._callback_on_trans_change(self)

    transformation = property(_get_transformation,_set_transformation)

    def __del__(self):
        self._callback_on_delete(self)


class GeometryFactory:

    def __init__(self,material_factory: MaterialFactory):
        self._state = "init"  # "geometry_loaded", "instances_loaded"
        self._tri_verticies = np.empty((0,3),dtype=np.float32)
        self._tri_normals = np.empty((0,3),dtype=np.float32)
        self._tri_uvs = np.empty((0,2),dtype=np.float32)
        self._tri_verticies_ids = np.empty((0,3),dtype=np.uint32)
        self._tri_normals_ids = np.empty((0,3),dtype=np.uint32)
        self._tri_uvs_ids = np.empty((0,3),dtype=np.uint32)
        self._tri_material_slot_ids = np.empty(0,dtype=np.uint32)
        self._tri_mesh_ends = []

        self._sphere_pos = np.empty((0,3),dtype=np.float32)
        self._sphere_radii = np.empty((0),dtype=np.float32)
        self._sphere_mat_ids = np.empty(0,dtype=np.uint32)
        self._created_instances = []
        self._sphere_mesh_ends = []

        self._c_sphere_bvhtree = None
        self._c_tri_bvhtree = None
        self._c_instances_bvhtree = None
        self._matfac = material_factory

    def spheres(self,positions, radii,material_slots):

        self._sphere_pos = np.concatenate([self._sphere_pos,positions],dtype=np.float32)
        self._sphere_radii = np.concatenate([self._sphere_radii,radii],dtype=np.float32)
        material_slots = np.array(material_slots,dtype=np.uint32)
        self._sphere_mat_ids = np.concatenate(
            [self._sphere_mat_ids,material_slots],dtype=np.uint32)
        end = len(self._sphere_pos)

        self._sphere_mesh_ends.append(end)
        id = len(self._sphere_mesh_ends) - 1

        inst = Instance(id,JPC_SPHERE,len(np.unique(material_slots)),
                        self._register_for_change,self.delete_instance)
        self._created_instances.append(inst)
        self._state = "init"
        return inst

    """
    vertices shape          = (a,3)
    uvs shape               = (b,2)
    normals shape           = (c,3)
    verticies_ids shape     = (n,3)
    normal_ids shape        = (n,3)
    uvs_ids shape           = (n,3)
    material_slot_ids shape = (n)
    """

    def triangles(self,vertices, uvs, normals, verticies_ids, normals_ids, uvs_ids, material_slot_ids):
        vertices = np.array(vertices,dtype=np.float32)
        print("type: ",vertices.dtype)
        uvs = np.array(uvs,dtype=np.float32)
        normals = np.array(normals,dtype=np.float32)
        verticies_ids = np.array(verticies_ids,dtype=np.uint32)
        normals_ids = np.array(normals_ids,dtype=np.uint32)
        uvs_ids = np.array(uvs_ids,dtype=np.uint32)
        material_slot_ids = np.array(material_slot_ids,dtype=np.uint32)
        self._tri_verticies = np.concatenate([
            self._tri_verticies,vertices],dtype=np.float32)
        self._tri_normals = np.concatenate([self._tri_normals,normals],dtype=np.float32)
        self._tri_uvs = np.concatenate([self._tri_uvs,uvs],dtype=np.float32)
        self._tri_verticies_ids = np.concatenate([
            self._tri_verticies_ids,verticies_ids],dtype=np.uint32)
        self._tri_uvs_ids = np.concatenate([self._tri_uvs_ids,uvs_ids],dtype=np.uint32)
        self._tri_material_slot_ids = np.concatenate([
            self._tri_material_slot_ids,material_slot_ids],dtype=np.uint32)
        self._tri_normals_ids = np.concatenate([
            self._tri_normals_ids,normals_ids],dtype=np.uint32)
        end = len(self._tri_verticies_ids)

        self._tri_mesh_ends.append(end)
        id = len(self._tri_mesh_ends) - 1

        inst = Instance(id,JPC_TRIANGLE,len(np.unique(material_slot_ids)),
                        self._register_for_change,self.delete_instance)
        self._created_instances.append(inst)
        self._state = "init"
        return inst

    def _register_for_change(self,instance):
        if(self._state == "instances_loaded"):
            self._state = "geometry_loaded"

    def delete_instance(self, instance):
        try:
            self._created_instances.remove(instance)
        except ValueError:
            return
        if(self._state == "instances_loaded"):
            self._state = "geometry_loaded"

    def copy_instance(self,instance):
        if(instance not in self._created_instances):
            raise ValueError("Illegal instance")
        inst2 = copy.copy(instance)
        self._created_instances.append(inst2)
        return inst2

    def _free_bvhtrees(self):
        if(self._c_tri_bvhtree is not None):
            ctracer.bvhtree_free(self._c_tri_bvhtree)
            self._c_tri_bvhtree = None
        if(self._c_sphere_bvhtree is not None):
            ctracer.bvhtree_free(self._c_sphere_bvhtree)
            self._c_sphere_bvhtree = None
        if(self._c_instances_bvhtree is not None):
            ctracer.bvhtree_free(self._c_instances_bvhtree)
            self._c_instances_bvhtree = None

    def __del__(self):
        self._free_bvhtrees()

    def _build_geometries(self):
        float_p = ct.POINTER(ct.c_float)
        uint_p = ct.POINTER(ct.c_uint)
        if(self._state == "init"):
            self._c_geometris = geometries_t()

            tris = triangles_t()
            tris.verticies_count = len(self._tri_verticies)
            tris.verticies = self._tri_verticies.ctypes.data_as(float_p)
            tris.normal_count = len(self._tri_normals)
            tris.normals = self._tri_normals.ctypes.data_as(float_p)
            tris.uvs_count = len(self._tri_uvs)
            tris.uvs = self._tri_uvs.ctypes.data_as(float_p)

            tris.faces_count = len(self._tri_normals_ids)
            tris.verticies_ids = self._tri_verticies_ids.ctypes.data_as(uint_p)
            tris.normals_ids = self._tri_normals_ids.ctypes.data_as(uint_p)
            tris.uvs_ids = self._tri_uvs_ids.ctypes.data_as(uint_p)
            tris.material_slots = self._tri_material_slot_ids.ctypes.data_as(uint_p)

            tris.mesh_count = len(self._tri_mesh_ends)
            self._c_tri_mesh_ends = np.array(self._tri_mesh_ends,dtype=np.uint)
            tris.mesh_end_idx = self._c_tri_mesh_ends.ctypes.data_as(uint_p)

            sphs = spheres_t()
            sphs.count = len(self._sphere_pos)
            sphs.positions = self._sphere_pos.ctypes.data_as(float_p)
            sphs.radii = self._sphere_radii.ctypes.data_as(float_p)
            sphs.material_slot_id = self._sphere_mat_ids.ctypes.data_as(uint_p)

            sphs.mesh_count = len(self._sphere_mesh_ends)
            self._c_sphere_mesh_ends = np.array(self._sphere_mesh_ends,dtype=np.uint32)
            sphs.mesh_end_idx = self._c_sphere_mesh_ends.ctypes.data_as(uint_p)

            self._c_geometris.triangles = tris
            self._c_geometris.spheres = sphs

            self._free_bvhtrees()
            self._c_tri_bvhtree = ctracer.bvhtree_build_triangles(tris)
            self._c_sphere_bvhtree = ctracer.bvhtree_build_spheres(sphs)
            self._c_geometris.bvhtree_triangles = self._c_tri_bvhtree
            self._c_geometris.bvhtree_spheres = self._c_sphere_bvhtree
            self._state = "geometry_loaded"
        if(self._state == "geometry_loaded"):
            inst_n = len(self._created_instances)
            self._c_instances = (instance_t * inst_n)()
            self._c_matsolts = np.array(
                [self._matfac.get_id(mat)
                 for inst in self._created_instances for mat in inst.materials],
                dtype=np.uint
                )
            print("material_solts:",self._c_matsolts)
            counter_mat = 0

            for i, inst in enumerate(self._created_instances):
                self._c_instances[i].type = inst._type
                self._c_instances[i].mesh_id = inst._id
                self._c_instances[i].material_slot_start = counter_mat
                print("material_slot_start: ",counter_mat)
                counter_mat += len(inst.materials)
                py_trans_p = inst.transformation.ctypes.data_as(ct.POINTER(ct.c_float))
                ct.memmove(self._c_instances[i].transformations,
                           py_trans_p,
                           ct.sizeof(ct.c_float) * 16)
            self._c_geometris.instances_count = inst_n
            self._c_geometris.instances = self._c_instances
            self._c_geometris.material_slots_count = len(self._c_matsolts)
            self._c_geometris.material_slots = self._c_matsolts.ctypes.data_as(
                ct.POINTER(ct.c_uint))
            if(self._c_instances_bvhtree is not None):
                ctracer.bvhtree_free(self._c_instances_bvhtree)

            self._c_instances_bvhtree = ctracer.bvhtree_build_instances(
                self._c_geometris)
            self._c_geometris.bvhtree_instances = self._c_instances_bvhtree
            self._state = "instances_loaded"

        return self._c_geometris


def RotationX(angle):
    angle = np.deg2rad(angle)
    return np.array(
        [[1,0,0,0],
         [0,math.cos(angle),math.sin(angle),0],
         [0,math.sin(angle),math.cos(angle),0],
         [0,0,0,1]],
        dtype=np.float32
        ).T.copy()


def RotationY(angle):

    angle = np.deg2rad(angle)
    return np.array(
        [[math.cos(angle),0,math.sin(angle),0],
         [0,1,0,0],
         [-math.sin(angle),0,math.cos(angle),0],
         [0,0,0,1]],
        dtype=np.float32).T.copy()


def RotationZ(angle):
    angle = np.deg2rad(angle)
    return np.array(
        [[math.cos(angle),-math.sin(angle),0,0],
         [math.sin(angle),math.cos(angle),0,0],
         [0,0,1,0],
         [0,0,0,1]],
        dtype=np.float32).T.copy()


def Translation(x,y,z):
    return np.array([
        [1,0,0,x],
        [0,1,0,y],
        [0,0,1,z],
        [0,0,0,1]],
        dtype=np.float32).T.copy()


def Scale(x,y,z):
    return np.array(
        [[x,0,0,0],
         [0,y,0,0],
         [0,0,z,0],
         [0,0,0,1]],
        dtype=np.float32).T.copy()


def RotSclTrans(rotation=[0,0,0], scale=[1,1,1], translation=[0,0,0]):
    if(type(scale) == float or type(scale) == int):
        scale = [scale,scale,scale]
    return RotationX(rotation[0])\
        .dot(RotationY(rotation[1]))\
        .dot(RotationZ(rotation[2]))\
        .dot(Scale(*scale))\
        .dot(Translation(*translation))
