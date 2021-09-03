
import cProfile
import pstats

import jpctracer.Material as sh
from jpctracer.Texture import Texture

profiler = cProfile.Profile()

profiler.enable()
fac = sh.MaterialFactory()

for i in range(1000):
    mat = fac.create("Diffuse")

    tex = Texture([[0,2],[0,4]])

    mat.roughness = tex

lib = fac._build_matlib()

#print("texture count: ", lib.textures_count)

mat.roughness = 0

#print(fac._created_materials)
print(fac._materials_to_update)

lib = fac._build_matlib()

print("texture count: ",lib.textures_count)

fac.delete(mat)

lib = fac._build_matlib()

print("texture count: ",lib.textures_count)
print("Materials: ",lib.materials_n)
profiler.disable()
stats = pstats.Stats(profiler).sort_stats('ncalls')
stats.print_stats()
