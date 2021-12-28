from jpctracer.Renderer import Renderer

import matplotlib.pyplot as plt

jpc = Renderer()

jpc.pointlight([4,4,-5],color=[0.1,0.1,0.1,1])
mesh = jpc.spheres([[0,0,-5]],[1,1,1])
mesh.materials[0] = jpc.Shaders.create("Diffuse")

jpc.settings(tile_size=20,subpixels=4,max_depth=1)
print("Start Render python")
result = jpc.render(20,20)
print("End Render python")

result = (result -result.min())/(result.max()-result.min())
plt.imshow(result)
plt.savefig("test.png")
print("End")

