from jpctracer.types import ctracer, shader_t, shaders_t
import ctypes as ct


ctracer.shaders_init.argtypes = []
ctracer.shaders_init.restype = shaders_t
ctracer.shaders_load_defaults.argtypes = [shaders_t]

ctracer.shaders_free.argtype = [shaders_t]


class ShadersFactory:
    _c_shaders = None

    shader_names = None
    
    def __init__(self):
        self._c_shaders = ctracer.shaders_init()
        ctracer.shaders_load_defaults(self._c_shaders)
        shaders = self._c_shaders
        self.shader_names = [] 
        for i in range(shaders.count):
            self.shader_names.append(
                    shaders.shaders[i].name.decode("utf-8")
                    )

    def __del__(self):
        ctracer.shaders_free(self._c_shaders)
             

        

class Shader:
    _c_shader = None

    name = None

    property_names = None

    property_types = None

    def __init__(self, shader):
        pass


