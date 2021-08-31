import ctypes as ct
ctracer = ct.PyDLL("build/jpctracer/c/libtracer.so")

class Ray(ct.Structure):
   _fields_ = [("origin",ct.c_float*3),
               ("direction",ct.c_float*3)];

class Camera(ct.Structure):
    _fields_ = [("near_plane",ct.c_float),
                ("position",ct.c_float*3),
                ("direction",ct.c_float*3)]

