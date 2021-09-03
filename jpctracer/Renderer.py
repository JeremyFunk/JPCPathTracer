import numpy as np
from jpctracer.Material import MaterialFactory

class Renderer:
    shaders = None

    def __init__(self):
        self.shaders = MaterialFactroy()

    def loadobj(self, path):
        pass

    def sphere(self, position, radius):
        pass

    def pointlight(self, position, intensity, falloff):
        pass

    def sunlight(self, direction, direction):
        pass

    def loadtexture(self,path):
        pass

    def texture(self,data):
        pass

    def settings(tile_size=16,subpixels=5,light_samples=20,max_depth=5):
        pass

    def render():
        pass


    shaders = MaterialFactory()  
