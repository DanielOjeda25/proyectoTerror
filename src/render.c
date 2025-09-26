// render.c - Sistema de renderizado 3D para Backrooms
#include "render.h"
#include "player.h"
#include "map.h"
#include <stdio.h>
#include <math.h>
#include <time.h>

// Definir M_PI si no está definido
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void init_renderer() {
    // Habilitar características 3D
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_TEXTURE_2D);
}

void setup_camera() {
    glLoadIdentity();
    
    // Calcular posición de la cámara
    float camX = player.x;
    float camY = player.y + player.height;
    float camZ = player.z;
    
    // Calcular dirección de la cámara basada en yaw y pitch
    float lookX = cos(player.yaw) * cos(player.pitch);
    float lookY = sin(player.pitch);
    float lookZ = sin(player.yaw) * cos(player.pitch);
    
    // Configurar la cámara
    gluLookAt(camX, camY, camZ,                    // Posición de la cámara
              camX + lookX, camY + lookY, camZ + lookZ,  // Punto al que mira
              0.0f, 1.0f, 0.0f);                   // Vector "arriba"
}

void draw_cube(float x, float y, float z, float size) {
    float half = size * 0.5f;
    
    glBegin(GL_QUADS);
    
    // Cara frontal (Z+)
    glColor3f(0.8f, 0.8f, 0.8f);
    glVertex3f(x - half, y - half, z + half);
    glVertex3f(x + half, y - half, z + half);
    glVertex3f(x + half, y + half, z + half);
    glVertex3f(x - half, y + half, z + half);
    
    // Cara trasera (Z-)
    glColor3f(0.7f, 0.7f, 0.7f);
    glVertex3f(x - half, y - half, z - half);
    glVertex3f(x - half, y + half, z - half);
    glVertex3f(x + half, y + half, z - half);
    glVertex3f(x + half, y - half, z - half);
    
    // Cara izquierda (X-)
    glColor3f(0.6f, 0.6f, 0.6f);
    glVertex3f(x - half, y - half, z - half);
    glVertex3f(x - half, y - half, z + half);
    glVertex3f(x - half, y + half, z + half);
    glVertex3f(x - half, y + half, z - half);
    
    // Cara derecha (X+)
    glColor3f(0.5f, 0.5f, 0.5f);
    glVertex3f(x + half, y - half, z - half);
    glVertex3f(x + half, y + half, z - half);
    glVertex3f(x + half, y + half, z + half);
    glVertex3f(x + half, y - half, z + half);
    
    // Cara superior (Y+)
    glColor3f(0.9f, 0.9f, 0.9f);
    glVertex3f(x - half, y + half, z - half);
    glVertex3f(x - half, y + half, z + half);
    glVertex3f(x + half, y + half, z + half);
    glVertex3f(x + half, y + half, z - half);
    
    // Cara inferior (Y-)
    glColor3f(0.4f, 0.4f, 0.4f);
    glVertex3f(x - half, y - half, z - half);
    glVertex3f(x + half, y - half, z - half);
    glVertex3f(x + half, y - half, z + half);
    glVertex3f(x - half, y - half, z + half);
    
    glEnd();
}

void draw_floor() {
    glBegin(GL_QUADS);
    glColor3f(0.2f, 0.2f, 0.2f); // Gris muy oscuro para backrooms
    glVertex3f(-50.0f, -0.1f, -50.0f);
    glVertex3f(50.0f, -0.1f, -50.0f);
    glVertex3f(50.0f, -0.1f, 50.0f);
    glVertex3f(-50.0f, -0.1f, 50.0f);
    glEnd();
}

void draw_ceiling() {
    glBegin(GL_QUADS);
    glColor3f(0.1f, 0.1f, 0.1f); // Negro para backrooms
    glVertex3f(-50.0f, 3.0f, -50.0f);
    glVertex3f(-50.0f, 3.0f, 50.0f);
    glVertex3f(50.0f, 3.0f, 50.0f);
    glVertex3f(50.0f, 3.0f, -50.0f);
    glEnd();
}

void render_world() {
    // Configurar la cámara
    setup_camera();
    
    // Dibujar suelo y techo
    draw_floor();
    draw_ceiling();
    
    // Renderizar el laberinto 3D
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            if (maze[y][x] == 1) { // Si hay una pared
                // Dibujar cubo en la posición (x, 0, y) con altura 3
                draw_cube(x, 1.5f, y, 1.0f);
            }
        }
    }
    
    // Dibujar algunos cubos decorativos para ambiente backrooms
    static bool decorAdded = false;
    if (!decorAdded) {
        // Añadir algunos cubos aleatorios para crear ambiente
        for (int i = 0; i < 20; i++) {
            int x = rand() % (MAZE_WIDTH - 2) + 1;
            int z = rand() % (MAZE_HEIGHT - 2) + 1;
            if (maze[z][x] == 0) { // Solo en espacios vacíos
                draw_cube(x, 0.5f, z, 0.3f);
            }
        }
        decorAdded = true;
    }
}

void cleanup_renderer() {
    // Limpiar recursos de renderizado (si los hay)
}
