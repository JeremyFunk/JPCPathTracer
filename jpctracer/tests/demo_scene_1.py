from jpctracer import renderer

jpc = renderer()

mesh1 = jpc.load_obj()

mesh2 = jpc.sphere([0,3,0],23)

jpc.settings()

result = jpc.render()


