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


mat_fac = MaterialFactory()
geom_fac_fac = GeometryFactory(mat_fac)
sp1 = geom_fac_fac.spheres([[0, 0, 4], [2, 3, 4], [-4, 4, 7]],
                           [1, 1, 0.5], [0, 0, 0])

sp2 = geom_fac_fac.spheres([[0, 0, 0]], [1], [0])


sp1.materials[0] = mat_fac.create("Diffuse")
sp2.materials[0] = mat_fac.create("Diffuse")

sp2.transformation = RotSclTrans(translation=[0, 0, 15.5], scale=[2, 2, 2])
print("transformation: \n", sp2.transformation)
print("transformation inv: \n", np.linalg.inv(sp2.transformation))

c_geom = geom_fac_fac._build_geometries()
print("Geometries")

tmp = ct.cast(c_geom.bvhtree_spheres, ct.POINTER(ct.c_int))

print(c_geom.bvhtree_spheres)
print(type(c_geom.bvhtree_spheres))

print("cast")


class hit_point_t(ct.Structure):
    _fields_ = [("instance_id", ct.c_uint),
                ("mesh_id", ct.c_uint),
                ("distance", ct.c_float),
                ("location", ct.c_float * 4),
                ("material_id", ct.c_int),
                ("uv", ct.c_float * 2),
                ("normal", ct.c_float * 3)]


ctracer.ray_intersect_c3.argtypes = [ct.POINTER(geometries_t),
                                  ct.POINTER(Ray),
                                  ct.POINTER(hit_point_t)]
ctracer.ray_intersect_c3.restype = ct.c_bool

img = np.empty(shape=(300, 300))

dir = np.array([0, 0, 1], dtype=np.float32)

hitpoint = hit_point_t()


ray = Ray()

ray.direction[0] = dir[0]
ray.direction[1] = dir[1]
ray.direction[2] = dir[2]
ray.origin[0] = 0
ray.origin[1] = 0
ray.origin[2] = 0
ray.clip_end = 14

print("hit p:", ctracer.ray_intersect(c_geom, ray, hitpoint))

loc = np.array(
    [hitpoint.location[0], hitpoint.location[1], hitpoint.location[2]])
org = np.array([0, 0, 0])
dist = np.linalg.norm(loc - org)
print("dist: ", dist)

for i_y, y in enumerate(np.arange(-10, 10, 20. / img.shape[0])):
    for i_x, x in enumerate(np.arange(-10, 10, 20. / img.shape[1])):
        ray.origin[0] = x
        ray.origin[1] = y
        ray.origin[2] = 0
        ray.clip_end = 14
        result = ctracer.ray_intersect_c3(c_geom, ray, hitpoint)
        if(result):
            loc = np.array(
                [hitpoint.location[0], hitpoint.location[1], hitpoint.location[2]])
            org = np.array([x, y, 0])
            dist = np.linalg.norm(loc - org)

            img[i_y][i_x] = dist
        else:
            img[i_y][i_x] = 0
        if(np.abs(y + 4) < 0.1 and np.abs(x + 4) < 0.1):
            img[i_y][i_x] = 0.5


#plt.imshow(np.clip(img - 13,0,10))
plt.imshow(img)
plt.savefig("load_spheres.png")
# plt.show()
# """
