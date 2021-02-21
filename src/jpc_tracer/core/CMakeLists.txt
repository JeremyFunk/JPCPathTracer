set(AUTOSRC
"Bsdf.cpp"
"FilmTile.cpp"
"JPCPathtracerApi.cpp"
"Logger.cpp"
"main.cpp"
"MonteCarlo.cpp"
"SpectrumPasses.cpp"
"Transformation.cpp"
"Base.h"
"Bounds2D.h"
"Bounds3D.h"
"Bsdf.h"
"BsdfGeneric.h"
"constants.h"
"FilmTile.h"
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
"JPCPathtracerApi.h"
"Linalg.h"
"Logger.h"
"MonteCarlo.h"
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
spdlog
)


jpctr_add_library(core "${AUTOSRC}" "${INC}" "${LIBS}" STATIC)

