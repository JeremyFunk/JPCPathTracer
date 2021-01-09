set(AUTOSRC
"FilmTile.cpp"
"main.cpp"
"MemoryArea.cpp"
"SpectrumPasses.cpp"
"Transformation.cpp"
"Bounds2D.h"
"Bounds3D.h"
"constants.h"
"FilmTile.h"
"IBSDF.h"
"ICamera.h"
"IFilm.h"
"IFilter.h"
"IIntegrator.h"
"ILight.h"
"IMaterial.h"
"IRenderer.h"
"ISampler.h"
"IScene.h"
"IShape.h"
"Linalg.h"
"MemoryArea.h"
"Ray.h"
"Spectrum.h"
"SpectrumPasses.h"
"SurfaceInteraction.h"
"SurfaceProperties.h"
"Transformation.h"
)




set(INC 
"../")

set(LIBS 
eigen3
)


jpctr_add_library(core "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

