#pragma once
#include "config.h"
#include <cglm/cglm.h>

void p_log_cube(const char* name, vec3 verticies[8]);

void p_log_bounds(const char* name, const float* min, const float* max);

void geom_log_write(const char* filepath);

#ifdef LOG_GEOMETRY
#define LOG_CUBE(name, verts) p_log_cube(name, verts);
#define LOG_BOUNDS(name, min, max) p_log_bounds(name, min, max);
#else

#define LOG_CUBE(name, verts)
#define LOG_BOUNDS(name, min, max)
#endif
