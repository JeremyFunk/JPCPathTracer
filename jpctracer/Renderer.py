import numpy as np
from jpctracer.Material import MaterialFactory, LoadTexture,Texture
from jpctracer.Lights import Lights
from jpctracer.Geometry import GeometryFactory
from jpctracer.types import camera_t, image_t, render_settings_t, scene_t,ctracer
import ctypes as ct

ctracer.render.argtypes = [ct.POINTER(scene_t),render_settings_t,ct.POINTER(image_t)]


class Renderer:

    def __init__(self):
        self.Shaders = MaterialFactory()
        self._lights = Lights()
        self._geometries = GeometryFactory(self.Shaders)
        self.settings()
        self.camera()


    def loadobj(self, path):
        pass

    def spheres(self, positions, radii, material_slots = None):
        if material_slots is None:
            material_slots = [0]*len(radii)
        return self._geometries.spheres(positions,radii,material_slots)
        

    def pointlight(self, position ,color=[1,1,1,1], intensity=1, falloff=1):
        return self._lights.add_pointlight(color,intensity,falloff,position)

    def sunlight(self, direction, color =[1,1,1,1],intensity=1):
        return self._lights.add_sunlight(color,intensity,direction)

    def loadtexture(self,path):
        return LoadTexture(path)

    def texture(self,data):
        return Texture(data)
    
    def camera(self,near_plane=1,position=[0,0,0],direction=[0,0,-1],clip_end=400):
        self._c_camera = camera_t()
        self._c_camera.near_plane=1
        for i in range(3):
            self._c_camera.position[i] = position[i]
            self._c_camera.direction[i] = direction[i]
        self._c_camera.clip_end = clip_end
        

    def settings(self,tile_size=16,subpixels=16,light_samples=32,max_depth=5):
        self._settings = render_settings_t()
        self._settings.tile_size = tile_size
        self._settings.subpixels = subpixels
        self._settings.light_samples = light_samples
        self._settings.max_depth = max_depth


    def render(self,width,height):
        scene = scene_t()
        scene.geometries = self._geometries._build_geometries()
        scene.materiallib = self.Shaders._build_matlib()
        scene.camera = self._c_camera
        scene.lights = self._lights._get_ctype()
        
        result = np.empty(shape=(width,height,4),dtype=np.float32)

        result_image = image_t()
        result_image.data = result.ctypes.data_as(ct.POINTER(ct.c_float))
        result_image.width=width
        result_image.height=height
        result_image.channels = 4

        ctracer.render(scene,self._settings,result_image)

        return result


        



