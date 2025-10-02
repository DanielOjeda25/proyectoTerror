// particles.c - Sistema simple de efectos de partículas
#include "particles.h"
#include <math.h>
#include <stdlib.h>

Particle particles[MAX_PARTICLES];
int particle_count = 0;

void init_particles() {
    particle_count = 0;
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].life = 0.0f;
    }
}

void update_particles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].life > 0.0f) {
            // Actualizar posición
            particles[i].x += particles[i].vx;
            particles[i].y += particles[i].vy;
            particles[i].z += particles[i].vz;
            
            // Aplicar gravedad
            particles[i].vy -= 0.01f;
            
            // Reducir vida
            particles[i].life -= 0.016f; // Aproximadamente 60 FPS
            
            // Reducir tamaño
            particles[i].size *= 0.99f;
            
            // Reducir alpha
            particles[i].a *= 0.98f;
            
            // Si la partícula muere, marcar para reutilización
            if (particles[i].life <= 0.0f) {
                particles[i].life = 0.0f;
            }
        }
    }
}

void render_particles() {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].life > 0.0f) {
            glPushMatrix();
            glTranslatef(particles[i].x, particles[i].y, particles[i].z);
            
            glColor4f(particles[i].r, particles[i].g, particles[i].b, particles[i].a);
            
            // Dibujar partícula como un pequeño cuadrado
            float size = particles[i].size;
            glBegin(GL_QUADS);
            glVertex3f(-size, -size, 0);
            glVertex3f(size, -size, 0);
            glVertex3f(size, size, 0);
            glVertex3f(-size, size, 0);
            glEnd();
            
            glPopMatrix();
        }
    }
    
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

void add_particle(float x, float y, float z, float vx, float vy, float vz) {
    // Buscar una partícula libre
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].life <= 0.0f) {
            particles[i].x = x;
            particles[i].y = y;
            particles[i].z = z;
            particles[i].vx = vx;
            particles[i].vy = vy;
            particles[i].vz = vz;
            particles[i].life = 1.0f;
            particles[i].size = 0.1f + (rand() % 10) / 100.0f;
            particles[i].r = 0.8f + (rand() % 20) / 100.0f;
            particles[i].g = 0.6f + (rand() % 20) / 100.0f;
            particles[i].b = 0.2f + (rand() % 20) / 100.0f;
            particles[i].a = 1.0f;
            break;
        }
    }
}

void cleanup_particles() {
    particle_count = 0;
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].life = 0.0f;
    }
}
