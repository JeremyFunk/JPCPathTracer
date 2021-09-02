from jpctracer.types import ctracer, shader_t,shaders_t, JPC_float,JPC_float3,JPC_float4
from jpctracer.Texture import Texture
import ctypes as ct
import numpy as np


ctracer.shaders_init.argtypes = []
ctracer.shaders_init.restype = shaders_t
ctracer.shaders_load_defaults.argtypes = [shaders_t]

ctracer.shaders_free.argtypes = [shaders_t]

ctracer.shader_default_uniform.argtypes = [ct.POINTER(shader_t),ct.c_uint,
                                           np.ctypeslib.ndpointer(ct.c_float)]


def size_of_uniform(uniform):
    if(uniform == JPC_float):
        return 1
    if(uniform == JPC_float3):
        return 3
    if(uniform == JPC_float4):
        return 4
    return -1




class Material(object):

    class Property:
        def __init__(self,type,value):
            self.type = type
            self.value = value

    _c_shader = None

    shader = ""

    _properties = {}

    def __init__(self, shader: shader_t):
        print("Init Mat")
        self._c_shader = shader
        self._property_types = []
        self._properties = {}
        self.shader = shader.name.decode("utf-8")

        for i in range(shader.uniforms_count):
            type = shader.uniforms_layout[i].type
            value = np.empty(size_of_uniform(type),dtype=np.float32)
            print("Fill prop")
            ctracer.shader_default_uniform(self._c_shader,i,value)
            prop = Material.Property(type,value)
            name = shader.uniforms_layout[i].name.decode("utf-8")

            self._properties[name] = prop

    def __getattr__(self, name: str):
        if(name not in self._properties):
            return super().__getattribute__(name)
        prop = self._properties[name]
        if(prop.type == JPC_float):
            if(type(prop.value) == np.ndarray):
                return prop.value[0]
            else:
                return prop.value[0]
        else:
            return prop.value

    def __setattr__(self,name:str,value):
        if(name not in self._properties):
            return super().__setattr__(name,value)

        prop = self._properties[name]
        type_size = size_of_uniform(prop.type)

        if(type(value) in [list,tuple]):
            value = np.array(value)

        if(type(value) == np.ndarray):

            if(not len(value) == size_of_uniform(prop.type)):
                raise ValueError(str(value) + " must be of length " + str(type_size))
            prop.value = value
        elif(type(value) == float or type(value) == int):
            prop.value = np.full(type_size,fill_value=value,
                                 dtype=np.float32)
        elif(type(value) == Texture):
            if(value.channels < type_size):
                raise ValueError("Texture must have at least " +
                                 str(type_size) + " channels")
            prop.value = value

    def property_names(self):
        return self._properties.keys()


class MaterialFactory:

    shader_names = []

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

    def create(self, shader_name):
        try:
            i = self.shader_names.index(shader_name)
            return Material(self._c_shaders.shaders[i])

        except ValueError:
            raise ValueError(shader_name + " is not a loaded Shader")
