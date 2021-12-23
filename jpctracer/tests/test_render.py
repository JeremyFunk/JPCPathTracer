from jpctracer.Renderer import Renderer

import matplotlib.pyplot as plt

jpc = Renderer()

jpc.pointlight([4,4,-5])
jpc.spheres([[0,0,-5]],[1])

result = jpc.render(200,200)

plt.imshow(result)