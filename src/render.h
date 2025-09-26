// render.h - Sistema de renderizado 3D para Backrooms
#ifndef RENDER_H
#define RENDER_H

#include <GL/gl.h>
#include <GL/glu.h>

// Funciones de renderizado 3D
void init_renderer();
void setup_camera();
void render_world();
void draw_cube(float x, float y, float z, float size);
void draw_floor();
void draw_ceiling();
void cleanup_renderer();

#endif // RENDER_H
