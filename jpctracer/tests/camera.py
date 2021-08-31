from ctypes import c_float
import numpy as np
from jpctracer.types import *
import matplotlib.pyplot as plt
print("hallo")
ctracer.generate_camera_ray.argtypes = [ct.POINTER(Camera),ct.c_int,ct.c_int,
np.ctypeslib.ndpointer(ct.c_float),
np.ctypeslib.ndpointer(ct.c_float)]
ctracer.generate_camera_ray.restype = Ray

cam = Camera()
cam.near_plane = 1



width = 1920
height = 1080

img = []

n_y = 0
n_x = 0

for y in range(0,height,10):
    n_x = 0
    for x in range(0,width,10):
        #print(x,y)
        pixel = np.array([x,y],dtype=np.float32)
        rand_p = np.array((0.5,0.3),dtype=np.float32)
        ray = ctracer.generate_camera_ray(cam,1920,1080,pixel,rand_p)
        img.append(np.ctypeslib.as_array(ray.direction))
        n_x+=1
    n_y +=1


img = np.array(img)
img.shape = (n_y,n_x,3)
img = img*0.5+0.5


plt.imshow(img)


plt.show()
