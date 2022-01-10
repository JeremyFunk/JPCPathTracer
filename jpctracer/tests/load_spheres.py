from jpctracer.Geometry import (
    GeometryFactory,
    MaterialFactory,
    RotSclTrans,
    )
from jpctracer.types import (
    ctracer,
    geometries_t,
    Ray,
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
sp1 = geom_fac_fac.spheres([[0,0,4],[2,3,4],[-4,4,7]],
                           [1,1,0.5],[0,0,0])

sp2 = geom_fac_fac.spheres([[0,0,0]],[1],[0])



sp1.materials[0] = mat_fac.create("Diffuse")
sp2.materials[0] = mat_fac.create("Diffuse")

sp2.transformation = RotSclTrans(translation=[0,0,15.5],scale=[2,2,2])
print("transformation: \n",sp2.transformation)
print("transformation inv: \n",np.linalg.inv(sp2.transformation))

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
                                    ct.POINTER(Ray),
                                  ct.POINTER(hit_point_t)]
ctracer.ray_intersect.restype = ct.c_bool

img = np.empty(shape=(300,300))

dir = np.array([0,0,1],dtype=np.float32)

hitpoint = hit_point_t()


ray = Ray()

ray.direction[0] = dir[0]
ray.direction[1] = dir[1]
ray.direction[2] = dir[2]
ray.origin[0] = 0
ray.origin[1] = 0
ray.origin[2] = 0
ray.clip_end = 14

print("hit p:",ctracer.ray_intersect(c_geom,ray,hitpoint))

loc = np.array(
    [hitpoint.location[0],hitpoint.location[1],hitpoint.location[2]])
org = np.array([0,0,0])
dist = np.linalg.norm(loc - org)
print("dist: ",dist)

for i_y,y in enumerate(np.arange(-10,10,20. / img.shape[0])):
    for i_x,x in enumerate(np.arange(-10,10,20. / img.shape[1])):
        ray.origin[0] = x
        ray.origin[1] = y
        ray.origin[2] = 0
        ray.clip_end = 14
        result = ctracer.ray_intersect(c_geom,ray,hitpoint)
        if(result):
            loc = np.array(
                [hitpoint.location[0],hitpoint.location[1],hitpoint.location[2]])
            org = np.array([x,y,0])
            dist = np.linalg.norm(loc - org)

            img[i_y][i_x] = dist
        else:
            img[i_y][i_x] = 0
        if(np.abs(y + 4) < 0.1 and np.abs(x + 4) < 0.1):
            img[i_y][i_x] = 0.5


#plt.imshow(np.clip(img - 13,0,10))
plt.imshow(img)
plt.savefig("load_spheres.png")
#plt.show()
#"""
