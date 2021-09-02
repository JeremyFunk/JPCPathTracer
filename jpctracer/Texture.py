from jpctracer.types import ctracer, image_t
import matplotlib.pyplot as plt


class Texture:
    channels = 0
    _data = None

    def __init__(self,data):
        if(not len(data.shape) in [2,3]):
            raise ValueError("image should be 2D or 3D dimensional")
        if(len(data.shape) == 2):
            self.channels = 1
        if(len(data.shape) == 3):
            self.channels = data.shape[2]
        self._data = data


def LoadTexture(path):
    return Texture(plt.imread(path))
