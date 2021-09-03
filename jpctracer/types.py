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
    pass

class image_t(ct.Structure):
    _fields_ = [("data",np.ctypeslib.ndpointer(ct.c_float)),
                ("width", ct.c_uint),
                ("height",ct.c_uint),
                ("channels",ct.c_uint)]

