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

// Variables de niebla
float fog_start = FOG_START_DISTANCE;
float fog_end = FOG_END_DISTANCE;
float fog_density = FOG_DENSITY;

// Variables de iluminación
float light_x = 0.0f;
float light_y = 0.0f;
float light_z = 0.0f;
float light_range = LIGHT_RANGE;

void init_renderer() {
    // Habilitar características 3D
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    // Configurar sistema de niebla
    setup_fog();
    
    // Configurar sistema de iluminación
    setup_lighting();
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
    
    // Configurar material para que responda a la iluminación
    GLfloat ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat diffuse[] = {0.7f, 0.7f, 0.7f, 1.0f};
    GLfloat specular[] = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat shininess[] = {10.0f};
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    
    glBegin(GL_QUADS);
    
    // Cara frontal (Z+)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(x - half, y - half, z + half);
    glVertex3f(x + half, y - half, z + half);
    glVertex3f(x + half, y + half, z + half);
    glVertex3f(x - half, y + half, z + half);
    
    // Cara trasera (Z-)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(x - half, y - half, z - half);
    glVertex3f(x - half, y + half, z - half);
    glVertex3f(x + half, y + half, z - half);
    glVertex3f(x + half, y - half, z - half);
    
    // Cara izquierda (X-)
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(x - half, y - half, z - half);
    glVertex3f(x - half, y - half, z + half);
    glVertex3f(x - half, y + half, z + half);
    glVertex3f(x - half, y + half, z - half);
    
    // Cara derecha (X+)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(x + half, y - half, z - half);
    glVertex3f(x + half, y + half, z - half);
    glVertex3f(x + half, y + half, z + half);
    glVertex3f(x + half, y - half, z + half);
    
    // Cara superior (Y+)
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(x - half, y + half, z - half);
    glVertex3f(x - half, y + half, z + half);
    glVertex3f(x + half, y + half, z + half);
    glVertex3f(x + half, y + half, z - half);
    
    // Cara inferior (Y-)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(x - half, y - half, z - half);
    glVertex3f(x + half, y - half, z - half);
    glVertex3f(x + half, y - half, z + half);
    glVertex3f(x - half, y - half, z + half);
    
    glEnd();
}

void draw_floor() {
    // Configurar material para el suelo
    GLfloat ambient[] = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat diffuse[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat specular[] = {0.05f, 0.05f, 0.05f, 1.0f};
    GLfloat shininess[] = {5.0f};
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f); // Normal hacia arriba
    glVertex3f(-200.0f, -0.1f, -200.0f);
    glVertex3f(200.0f, -0.1f, -200.0f);
    glVertex3f(200.0f, -0.1f, 200.0f);
    glVertex3f(-200.0f, -0.1f, 200.0f);
    glEnd();
}

void draw_ceiling() {
    // Configurar material para el techo
    GLfloat ambient[] = {0.05f, 0.05f, 0.05f, 1.0f};
    GLfloat diffuse[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat specular[] = {0.02f, 0.02f, 0.02f, 1.0f};
    GLfloat shininess[] = {2.0f};
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    
    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f); // Normal hacia abajo
    glVertex3f(-200.0f, 3.0f, -200.0f);
    glVertex3f(-200.0f, 3.0f, 200.0f);
    glVertex3f(200.0f, 3.0f, 200.0f);
    glVertex3f(200.0f, 3.0f, -200.0f);
    glEnd();
}

void render_world() {
    // Actualizar iluminación dinámica
    update_lighting();
    
    // Configurar la cámara
    setup_camera();
    
    // Dibujar suelo y techo
    draw_floor();
    draw_ceiling();
    
    // Renderizar el laberinto 3D con carga progresiva basada en la luz
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            if (maze[y][x] == 1) { // Si hay una pared
                // Calcular distancia desde el jugador
                float distance = sqrt((x - player.x) * (x - player.x) + 
                                     (y - player.z) * (y - player.z));
                
                // Solo renderizar si está dentro del rango de luz
                if (distance <= light_range) {
                    // Dibujar cubo en la posición (x, 0, y) con altura 3
                    draw_cube(x, 1.5f, y, 1.0f);
                }
            } else if (maze[y][x] == 2) { // Elementos decorativos
                // Calcular distancia desde el jugador
                float distance = sqrt((x - player.x) * (x - player.x) + 
                                     (y - player.z) * (y - player.z));
                
                // Solo renderizar si está dentro del rango de luz (más cercano)
                if (distance <= light_range * 0.8f) {
                    // Dibujar elemento decorativo más pequeño
                    draw_cube(x, 0.3f, y, 0.2f);
                }
            }
        }
    }
}

void setup_fog() {
    // Configurar niebla para crear ambiente de backrooms
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogf(GL_FOG_START, fog_start);
    glFogf(GL_FOG_END, fog_end);
    glFogf(GL_FOG_DENSITY, fog_density);
    
    // Color de la niebla (amarillo verdoso para backrooms)
    GLfloat fogColor[4] = {0.1f, 0.1f, 0.05f, 1.0f};
    glFogfv(GL_FOG_COLOR, fogColor);
}

void update_fog_distance() {
    // Actualizar distancia de niebla basada en la posición del jugador
    // La niebla se despeja gradualmente al moverse
    static float base_fog_start = FOG_START_DISTANCE;
    static float base_fog_end = FOG_END_DISTANCE;
    
    // Calcular distancia desde el centro del mapa
    float centerX = MAZE_WIDTH / 2.0f;
    float centerZ = MAZE_HEIGHT / 2.0f;
    float distance_from_center = sqrt((player.x - centerX) * (player.x - centerX) + 
                                     (player.z - centerZ) * (player.z - centerZ));
    
    // Ajustar niebla basada en la distancia del centro
    // Más lejos del centro = menos niebla
    float fog_factor = 1.0f - (distance_from_center / (MAZE_WIDTH / 2.0f));
    fog_factor = fmax(0.3f, fmin(1.0f, fog_factor)); // Limitar entre 0.3 y 1.0
    
    fog_start = base_fog_start * fog_factor;
    fog_end = base_fog_end * fog_factor;
    
    // Actualizar parámetros de niebla
    glFogf(GL_FOG_START, fog_start);
    glFogf(GL_FOG_END, fog_end);
}

void setup_lighting() {
    // Configurar luz ambiental (muy tenue para backrooms)
    GLfloat ambient[] = {0.1f, 0.1f, 0.1f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    
    // Configurar luz difusa (amarilla para ambiente backrooms)
    GLfloat diffuse[] = {0.8f, 0.8f, 0.6f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    
    // Configurar luz especular (muy tenue)
    GLfloat specular[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    
    // Configurar atenuación
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, LIGHT_ATTENUATION);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.1f);
    
    // Configurar rango de la luz
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0f); // Luz omnidireccional
}

void update_fog_based_on_lighting() {
    // La niebla se ajusta basada en la intensidad de la luz
    // En áreas más iluminadas, la niebla es menos densa
    float light_intensity = 1.0f;
    
    // Calcular intensidad basada en la distancia del centro
    float centerX = MAZE_WIDTH / 2.0f;
    float centerZ = MAZE_HEIGHT / 2.0f;
    float distance_from_center = sqrt((player.x - centerX) * (player.x - centerX) + 
                                     (player.z - centerZ) * (player.z - centerZ));
    
    // Ajustar intensidad de luz basada en la distancia
    float max_distance = sqrt(centerX * centerX + centerZ * centerZ);
    light_intensity = 1.0f - (distance_from_center / max_distance) * 0.3f;
    light_intensity = fmax(0.7f, fmin(1.0f, light_intensity));
    
    // Ajustar niebla basada en la intensidad de luz
    fog_start = FOG_START_DISTANCE * light_intensity;
    fog_end = FOG_END_DISTANCE * light_intensity;
    fog_density = FOG_DENSITY * (2.0f - light_intensity);
    
    // Actualizar parámetros de niebla
    glFogf(GL_FOG_START, fog_start);
    glFogf(GL_FOG_END, fog_end);
    glFogf(GL_FOG_DENSITY, fog_density);
}

void update_lighting() {
    // Actualizar posición de la luz para que siga a la cámara
    light_x = player.x;
    light_y = player.y + player.height + 0.5f; // Un poco por encima de la cabeza
    light_z = player.z;
    
    // Configurar posición de la luz
    GLfloat position[] = {light_x, light_y, light_z, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    
    // Actualizar niebla basada en la iluminación
    update_fog_based_on_lighting();
}

void cleanup_renderer() {
    // Limpiar recursos de renderizado (si los hay)
}
