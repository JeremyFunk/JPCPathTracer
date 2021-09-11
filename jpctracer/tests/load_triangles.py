
from jpctracer.Geometry import (
    GeometryFactory,
    MaterialFactory,
    RotSclTrans,
    RotationZ,
    )
from jpctracer.types import (
    ctracer,
    geometries_t,
    )
import ctypes as ct
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.colors as colors


class bvh_node_t(ct.Structure):
    _fields_ = [("first_idx",ct.c_uint),
                ("last_idx",ct.c_uint),
                ("split_idx",ct.c_uint)]


class bvh_tree_t(ct.Structure):
    _fields_ = [("n",ct.c_int),
                ("shape_bounds",ct.POINTER(ct.c_float)),
                ("nodes",ct.POINTER(bvh_node_t)),
                ("node_bounds",ct.POINTER(ct.c_float))]


mat_fac = MaterialFactory()
geom_fac_fac = GeometryFactory(mat_fac)

sp1 = geom_fac_fac.spheres([[-1.5,0,5],[1,0,5]],
                           [2,2],[0,0])
sp1.materials[0] = mat_fac.create("Diffuse")

tri = geom_fac_fac.triangles([[-0.5,-0.5,0],[-0.5,1.5,0],[1.5,-0.5,0]],
                             [[0,0]],[[1,1,1]],[[0,1,2]],[[0,0,0]],[[0,0,0]],[0])

tri.materials[0] = mat_fac.create("Diffuse")

tri.transformation = RotSclTrans(translation=[0,0,13],scale=6,rotation=[0,0,90])

print("trans: ",tri.transformation)


c_geom = geom_fac_fac._build_geometries()
print("Geometries")

tmp = ct.cast(c_geom.bvhtree_spheres,ct.POINTER(ct.c_int))

print(c_geom.bvhtree_spheres)
print(type(c_geom.bvhtree_spheres))

print("cast")


def print_tree(tree_p):

    tree = ct.cast(tree_p,ct.POINTER(bvh_tree_t)).contents
    n = tree.n
    print("n: ",n)
    if(n == 0):
        return
    shape_bounds = np.ctypeslib.as_array(tree.shape_bounds,shape=(tree.n,2,3))
    node_bounds = np.ctypeslib.as_array(tree.node_bounds,shape=(tree.n - 1,2,3))
    print("shape_bounds:\n",np.round(shape_bounds,decimals=3))
    print("node_bounds:\n",np.round(node_bounds,decimals=3))
    for i in range(n - 1):
        print("node")
        print("first idx: ",tree.nodes[i].first_idx)
        print("last_idx: ",tree.nodes[i].last_idx)
        print("split idx: ",tree.nodes[i].split_idx)


print("------Instances-----")
print_tree(c_geom.bvhtree_instances)
print("------Spheres------")
print_tree(c_geom.bvhtree_spheres)
print("----Tirangles----")
print_tree(c_geom.bvhtree_triangles)


class hit_point_t(ct.Structure):
    _fields_ = [("location",ct.c_float * 4),
                ("material_id",ct.c_int),
                ("uvs",ct.c_float * 2),
                ("normal",ct.c_float * 3)]


ctracer.ray_intersect.argtypes = [ct.POINTER(geometries_t),
                                  np.ctypeslib.ndpointer(ct.c_float),
                                  ct.POINTER(hit_point_t)]
ctracer.ray_intersect.restype = ct.c_bool

img = np.empty(shape=(300,300))

dir = np.array([0,0,14],dtype=np.float32)

hitpoint = hit_point_t()


org = np.array([0,0,0])
hitpoint.location[0] = org[0]
hitpoint.location[1] = org[1]
hitpoint.location[2] = org[2]
hitpoint.location[3] = 1

print("hit p:",ctracer.ray_intersect(c_geom,dir,hitpoint))

loc = np.array(
    [hitpoint.location[0],hitpoint.location[1],hitpoint.location[2]])
dist = np.linalg.norm(loc - org)
print("dist: ",dist)

#"""
for i_y,y in enumerate(np.arange(-10,10,20. / img.shape[0])):
    for i_x,x in enumerate(np.arange(-10,10,20. / img.shape[1])):
        hitpoint.location[0] = x
        hitpoint.location[1] = y
        hitpoint.location[2] = 0
        hitpoint.location[3] = 1
        result = ctracer.ray_intersect(c_geom,dir,hitpoint)
        if(result):
            loc = np.array(
                [hitpoint.location[0],hitpoint.location[1],hitpoint.location[2]])
            _org = np.array([x,y,0])
            dist = np.linalg.norm(loc - _org)

            img[i_y][i_x] = dist
        else:
            img[i_y][i_x] = 0
        if(np.abs(org[0] - x) < 0.1 and np.abs(org[0] - y) < 0.1):
            img[i_y][i_x] = 20


#plt.imshow(np.clip(img - 8.5,0,10))
plt.imshow(np.log(1 + img))
plt.show()
#"""
