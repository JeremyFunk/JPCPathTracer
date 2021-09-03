from jpctracer.types import (
    ctracer,
    materiallib_t,
    shader_t,
    shaders_t,
    JPC_float,
    JPC_float3,
    JPC_float4,
    material_t,
    image_t)
from jpctracer.Texture import Texture
import ctypes as ct
import numpy as np


ctracer.shaders_init.argtypes = []
ctracer.shaders_init.restype = shaders_t
ctracer.shaders_load_defaults.argtypes = [shaders_t]

ctracer.shaders_free.argtypes = [shaders_t]

ctracer.shader_default_uniform.argtypes = [ct.POINTER(shader_t),ct.c_uint,
                                           np.ctypeslib.ndpointer(ct.c_float)]
ctracer.material_set_uniform.argtypes = [ct.POINTER(material_t),ct.POINTER(shader_t),
                                         ct.c_uint, np.ctypeslib.ndpointer(ct.c_float)]
ctracer.material_set_texture.argtypes = [ct.POINTER(material_t),ct.POINTER(shader_t),
                                         ct.c_uint, ct.c_uint]

ctracer.mat_bfr_t_free.argtypes = [ct.c_void_p]
ctracer.materials_init.argtypes = [ct.POINTER(material_t)]
ctracer.materials_init.restype = ct.c_void_p


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
        def __init__(self,type,value,id):
            self.type = type
            self.value = value
            self.id = id

    _c_shader = None

    shader = ""

    _properties = {}

    def __init__(self, shader: shader_t, callback_on_prop_change,callback_on_delete):
        self._c_shader = shader
        self._property_types = []
        self._properties = {}
        self.shader = shader.name.decode("utf-8")
        self._callback_on_prop_change = callback_on_prop_change
        self._callback_on_delete = callback_on_delete

        for i in range(shader.uniforms_count):
            type = shader.uniforms_layout[i].type
            value = np.empty(size_of_uniform(type),dtype=np.float32)
            ctracer.shader_default_uniform(self._c_shader,i,value)
            prop = Material.Property(type,value,i)
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

        self._callback_on_prop_change(self)

    def property_names(self):
        return self._properties.keys()

    def __del__(self):
        self._callback_on_delete(self)


class materiallib:

    def __init__(self, materials):
        self._textures: list[Texture] = []
        py_shaders = [mat._c_shader for mat in materials]
        self._c_shaders = (shader_t * len(py_shaders))(*py_shaders)
        self._c_materials = (material_t * len(materials))()
        self._c_mat_bfr = ctracer.materials_init(self._c_materials,
                                                 self._c_shaders,len(materials))
        self._materials_count = len(materials)
        for i,mat in enumerate(materials):
            self.update_material(mat,i,False)

    def update_material(self,material: Material,idx, check_for_texture=True):
        c_material = self._c_materials[idx]
        c_shader = self._c_shaders[idx]
        for prop in material._properties.values():
            prop_i = prop.id
            if(type(prop.value) == np.ndarray):
                ctracer.material_set_uniform(c_material,c_shader,prop_i,prop.value)
            elif(type(prop.value) == Texture):

                tex_id = -1
                if(check_for_texture):
                    try:
                        tex_id = self._textures.index(prop.value)
                    except ValueError:
                        pass
                if(tex_id == -1):
                    self._textures.append(prop.value)
                    tex_id = len(self._textures) - 1
                ctracer.material_set_texture(c_material,c_shader,prop_i,tex_id)

    def get(self):
        self._c_textures = (image_t * len(self._textures))()
        for i,tex in enumerate(self._textures):
            c_tex = self._c_textures[i]
            c_tex.data = tex._data.ctypes.data_as(ct.POINTER(ct.c_float))
            c_tex.width = tex._data.shape[1]
            c_tex.height = tex._data.shape[0]
            c_tex.channels = tex.channels

        matlib = materiallib_t()
        matlib.materials_n = self._materials_count
        matlib.materials = self._c_materials
        matlib.buffer = self._c_mat_bfr
        matlib.textures = self._c_textures
        matlib.textures_count = len(self._textures)

        return matlib

    def __del__(self):
        ctracer.mat_bfr_t_free(self._c_mat_bfr)


class MaterialFactory:

    shader_names = []

    _created_materials = []
    _materials_to_update = []  # saves the ids

    def __init__(self):
        self._c_shaders = ctracer.shaders_init()
        ctracer.shaders_load_defaults(self._c_shaders)
        shaders = self._c_shaders
        self.shader_names = []
        self._matlib = None
        for i in range(shaders.count):
            self.shader_names.append(
                shaders.shaders[i].name.decode("utf-8")
                )

    def __del__(self):
        ctracer.shaders_free(self._c_shaders)

    def get_id(self,material):
        return self._created_materials.index(material)

    def delete(self, material):
        try:
            self._created_materials.remove(material)
        except ValueError:
            pass
        self._matlib = None

    def _register_for_update(self,material):
        if(self._matlib is None):
            return
        id = self.get_id(material)
        if(id not in self._materials_to_update):
            self._materials_to_update.append(id)

    def create(self, shader_name):
        try:
            i = self.shader_names.index(shader_name)
            mat = Material(self._c_shaders.shaders[i],
                           self._register_for_update,
                           self.delete)
            self._created_materials.append(mat)
            self._materials_to_update.append(len(self._created_materials) - 1)
            return mat

        except ValueError:
            raise ValueError(shader_name + " is not a loaded Shader")

    def _build_matlib(self):
        if(self._matlib is None):
            self._matlib = materiallib(self._created_materials)
        else:
            for i in self._materials_to_update:
                self._matlib.update_material(self._created_materials[i],i)
        self._materials_to_update = []
        return self._matlib.get()
