
from jpc import shader, texture, lights
from jpc import objloader

diff = shader("diffuse")

diff.alberto = texture("path")
diff.roughness = 0

cube = objloader.load("path")

cube.material[0] = diff

img = render([cube],[lights.default])

plt.show(img)