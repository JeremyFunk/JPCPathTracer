from jpctracer.Geometry import (
    GeometryFactory,
    MaterialFactory,
    )
from jpctracer.types import (
    ctracer,
    geometries_t,
    )
import ctypes as ct
import numpy as np
import matplotlib.pyplot as plt


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
sp1 = geom_fac_fac.spheres([[0,0,4],[2,3,4],[-4,4,7]],
                           [1,1,0.5],[0,0,0])

sp2 = geom_fac_fac.spheres([[-4,-4,8]],[1],[0])

sp1.materials[0] = mat_fac.create("Diffuse")
sp2.materials[0] = mat_fac.create("Diffuse")

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


hitpoint.location[0] = 2
hitpoint.location[1] = 3 
hitpoint.location[2] = 0
hitpoint.location[3] = 1

print("hit p:",ctracer.ray_intersect(c_geom,dir,hitpoint))
for i_y,y in enumerate(np.arange(-10,10,20. / img.shape[0])):
    for i_x,x in enumerate(np.arange(-10,10,20. / img.shape[1])):
        hitpoint.location[0] = x
        hitpoint.location[1] = y
        hitpoint.location[2] = 0
        hitpoint.location[3] = 1
        result = ctracer.ray_intersect(c_geom,dir,hitpoint)
        if(result):
            img[i_y][i_x] = 1
        else:
            img[i_y][i_x] = 0
        if(np.abs(y + 4) < 0.1 and np.abs(x + 4) < 0.1):
            img[i_y][i_x] = 0.5


plt.imshow(img)
plt.show()
