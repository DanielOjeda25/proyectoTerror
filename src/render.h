// render.h - Sistema de renderizado 3D para Backrooms
#ifndef RENDER_H
#define RENDER_H

#include <GL/gl.h>
#include <GL/glu.h>

// Constantes de niebla
#define FOG_START_DISTANCE 1.5f
#define FOG_END_DISTANCE 4.0f
#define FOG_DENSITY 0.95f

// Constantes de iluminación
#define LIGHT_RANGE 12.0f
#define LIGHT_INTENSITY 1.0f
#define LIGHT_ATTENUATION 0.3f

// Funciones de renderizado 3D
void init_renderer();
void setup_camera();
void render_world();
void draw_cube(float x, float y, float z, float size);
void draw_tall_wall(int x, int z, int levels);
void draw_floor();
void draw_ceiling();
void setup_fog();
void update_fog_distance();
void setup_lighting();
void update_lighting();
void update_fog_based_on_lighting();
void render_light_points();
void render_minimap();
void draw_minimap_background();
void draw_minimap_walls();
void draw_minimap_player();
void draw_minimap_lights();
void load_loading_texture();
void render_loading_screen();
void setup_additional_lights();
void cleanup_renderer();

// Variables de niebla
extern float fog_start;
extern float fog_end;
extern float fog_density;

// Variables de iluminación
extern float light_x;
extern float light_y;
extern float light_z;
extern float light_range;

#endif // RENDER_H
