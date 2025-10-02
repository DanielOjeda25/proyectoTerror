// particles.h - Sistema simple de efectos de partículas
#ifndef PARTICLES_H
#define PARTICLES_H

#include <GL/gl.h>

// Estructura para partículas
typedef struct {
    float x, y, z;
    float vx, vy, vz;
    float life;
    float size;
    float r, g, b, a;
} Particle;

// Sistema de partículas
#define MAX_PARTICLES 1000
extern Particle particles[MAX_PARTICLES];
extern int particle_count;

// Funciones de partículas
void init_particles();
void update_particles();
void render_particles();
void add_particle(float x, float y, float z, float vx, float vy, float vz);
void cleanup_particles();

#endif // PARTICLES_H
