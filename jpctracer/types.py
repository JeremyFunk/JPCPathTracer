import ctypes as ct
import numpy as np
ctracer = ct.PyDLL("build/jpctracer/c/libtracer.so")


class Ray(ct.Structure):
    _fields_ = [("origin",ct.c_float * 3),
                ("direction",ct.c_float * 3)]


class Camera(ct.Structure):
    _fields_ = [("near_plane",ct.c_float),
                ("position",ct.c_float * 3),
                ("direction",ct.c_float * 3)]


JPC_float = 0
JPC_float4 = 1
JPC_float3 = 2

JPC_SPHERE = 0
JPC_TRIANGLE = 1


class uniform_desc_t(ct.Structure):
    _fields_ = [("type",ct.c_int),
                ("name",ct.c_char_p)]


class shader_t(ct.Structure):
    _fields_ = [("name",ct.c_char_p),
                ("uniforms_layout",ct.POINTER(uniform_desc_t)),
                ("uniforms_count",ct.c_uint),
                ("uniforms_size",ct.c_size_t),
                ("uniforms_default",ct.c_void_p),
                ("create_bsdf_f",ct.c_void_p)]


class shaders_t(ct.Structure):
    _fields_ = [("shaders",ct.POINTER(shader_t)),
                ("count",ct.c_uint)]


class material_t(ct.Structure):
    _fields_ = [("bsdf_creator",ct.c_void_p),
                ("param_size",ct.c_uint),
                ("params",ct.c_void_p),
                ("bindings",ct.c_void_p),
                ("bindings_count",ct.c_uint)]


class image_t(ct.Structure):
    _fields_ = [("data",ct.POINTER(ct.c_float)),
                ("width", ct.c_uint),
                ("height",ct.c_uint),
                ("channels",ct.c_uint)]


class materiallib_t(ct.Structure):
    _fields_ = [("materials_n",ct.c_uint),
                ("materials",ct.POINTER(material_t)),
                ("buffer",ct.c_void_p),
                ("textures",ct.POINTER(image_t)),
                ("textures_count",ct.c_uint)]


class triangles_t(ct.Structure):
    _fields_ = [("verticies_count",ct.c_uint),
                ("verticies",ct.POINTER(ct.c_float)),  # shape (n,3)
                ("normal_count",ct.c_uint),
                ("normals",ct.POINTER(ct.c_float)),  # shape (n,3)
                ("uvs_count",ct.c_uint),
                ("uvs",ct.POINTER(ct.c_float)),  # shape (n,2)
                ("faces_count",ct.c_uint),
                ("verticies_ids",ct.POINTER(ct.c_uint)),  # shape (m,3)
                ("normals_ids",ct.POINTER(ct.c_uint)),  # shape (m,3)
                ("uvs_ids",ct.POINTER(ct.c_uint)),  # shape (m,2)
                ("material_slots",ct.POINTER(ct.c_uint)),  # shape (m)
                ("mesh_count",ct.c_uint),
                ("mesh_end_idx",ct.POINTER(ct.c_uint))  # shape (n)
                ]


class spheres_t(ct.Structure):
    _fields_ = [("count",ct.c_uint),
                ("positions",ct.POINTER(ct.c_float)),  # shape (n,3)
                ("radii",ct.POINTER(ct.c_float)),  # shape (n,3)
                ("material_slot_id",ct.POINTER(ct.c_uint)),  # shape (n,3)
                ("mesh_count",ct.c_uint),
                ("mesh_end_idx",ct.POINTER(ct.c_uint))  # shape (n)
                ]


class instance_t(ct.Structure):
    _fields_ = [("type",ct.c_int),
                ("mesh_id",ct.c_uint),
                ("material_slot_start",ct.c_uint),
                ("transformations",ct.c_float * 16)]


class geometries_t(ct.Structure):
    _fields_ = [("instances_count",ct.c_uint),
                ("instances",ct.POINTER(instance_t)),
                ("triangles",triangles_t),
                ("spheres",spheres_t),
                ("material_slots",ct.POINTER(ct.c_uint)),
                ("material_slots_count",ct.c_uint),
                ("bvhtree_instances",ct.c_void_p),
                ("bvhtree_triangles",ct.c_void_p),
                ("bvhtree_spheres",ct.c_void_p)]


