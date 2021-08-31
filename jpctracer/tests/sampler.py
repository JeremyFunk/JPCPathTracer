from ctypes import c_float
import numpy as np
from jpctracer.types import *
import matplotlib.pyplot as plt

ctracer.sampler_init.argtypes = []
ctracer.sampler_init.restype = ct.c_void_p
ctracer.sampler_free.argtypes = [ct.c_void_p]

ctracer.sample2d.argtypes = [ct.c_void_p,ct.c_int,ct.c_int,np.ctypeslib.ndpointer(ct.c_float)]

sampler = ctracer.sampler_init()


width = 1
height = 3000

samples = np.zeros(height*width*2,np.float32)

print("sample")
ctracer.sample2d(sampler,width,height,samples)


ctracer.sampler_free(sampler)

samples.shape = (height,width,2)
print("plot")
plt.scatter(samples[:,:,0].flatten(),samples[:,:,1].flatten(),s=0.1)
plt.show()

