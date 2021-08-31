import ctypes as ct
from jpctracer.types import ctracer
import numpy as np
import matplotlib.pyplot as plt


ctracer.sample_discrete.argtypes = [
    np.ctypeslib.ndpointer(ct.c_float),
    ct.c_int,
    np.ctypeslib.ndpointer(ct.c_float)
]
ctracer.sample_discrete.restype = ct.c_int

n = 5000

pdfs = np.array([0.4,0.1,0.2,0.3])
pdfs /= pdfs.sum()
pdfs = pdfs.astype(np.float32)

samples = np.random.random(n)
samples = samples.astype(np.float32)

sampled_ids = []

for i,sample in enumerate(samples):
    sample_p = np.array([sample])
    sample_p = sample_p.astype(np.float32)
    id = ctracer.sample_discrete(pdfs,n,sample_p)
    samples[i] = sample_p[0]
    sampled_ids.append(id)

sampled_ids = np.array(sampled_ids)
hist = []
for i in range(len(pdfs)):
    hist.append((sampled_ids == i).sum() / len(sampled_ids))

plt.bar(np.arange(len(pdfs)),hist)

plt.show()
plt.plot(samples,'o')
plt.show()

