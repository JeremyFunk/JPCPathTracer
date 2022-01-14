from jpctracer.Renderer import Renderer
import numpy as np

import matplotlib.pyplot as plt

jpc = Renderer()

light_pos = [4,4,-4]
jpc.pointlight(light_pos,color=[0.1,0.1,0.1,1],intensity=3e2)

jpc.pointlight([-4,4,-4],color=[0,0.1,0.1,1],intensity=3e2)
mesh = jpc.spheres([[0,0,-5]],[1,1,1])
mesh.materials[0] = jpc.Shaders.create("Diffuse")
mesh.materials[0].color = [0,0.1,0.1,1]

jpc.settings(tile_size=20,subpixels=4,max_depth=2)
print("Start Render python")
result = jpc.render(200,200)
print("End Render python")


result_no_alpha = result[:,:,[0,1,2]]
min = np.nanmin(result_no_alpha)
max = np.nanmax(result_no_alpha)
print("max: ",max)
print("min: ",min)
norm_result = (result-min)/(max-min)
norm_result[:,:,3]=1
plt.imshow(norm_result)
plt.savefig("test.png")
plt.show()




import seaborn as sb
"""
for i in range(4):
    sb.heatmap(result[:,:,i])
    plt.savefig("test" + str(i) + ".png")
    plt.clf()
print("End")
"""
