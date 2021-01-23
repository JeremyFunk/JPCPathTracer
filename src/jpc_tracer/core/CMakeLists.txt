set(AUTOSRC
"Bsdf.cpp"
"Transformation.cpp"
"FilmTile.cpp"
"main.cpp"
"SpectrumPasses.cpp"
"Linalg.h"
"SurfaceProperties.h"
"BsdfGeneric.h"
"IFilm.h"
"Transformation.h"
"Bounds2D.h"
"FilmTile.h"
"Bounds3D.h"
"IIntegrator.h"
"IMaterial.h"
"IRenderer.h"
"ILight.h"
"ISampler.h"
"SpectrumPasses.h"
"IScene.h"
"Ray.h"
"Spectrum.h"
"ICamera.h"
"IFilter.h"
"constants.h"
"Bsdf.h"
"IShape.h"
"SurfaceInteraction.h"
)




set(INC 
"../")

set(LIBS 
eigen3
)


jpctr_add_library(core "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

