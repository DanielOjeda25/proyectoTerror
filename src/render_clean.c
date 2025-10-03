// render.c - Sistema de renderizado 3D para Backrooms
#include "render.h"
#include "player.h"
#include "map.h"
#include "enemy.h"
#include "particles.h"
#include "image_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Variables globales del renderizador
int windowWidth = 1920;
int windowHeight = 1080;

// Variables de niebla
float fog_start = 15.0f;
float fog_end = 80.0f;
float fog_density = 0.003f;

// Variables de iluminación
float light_x = 0.0f;
float light_y = 0.0f;
float light_z = 0.0f;
float light_range = LIGHT_RANGE;

// Variables del sistema de carga eliminadas

void init_renderer() {
    // Habilitar características 3D
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_FOG);
    
    // Configurar culling
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    // Configurar sistema de iluminación
    setup_lighting();
    
    // Pantalla de carga eliminada
    
    // Inicializar sistema de partículas
    init_particles();
    
    // Inicializar tiempo para efectos dinámicos
}

void setup_camera() {
    glLoadIdentity();
    
    // Calcular dirección de la cámara basada en yaw y pitch
    float lookX = cos(player.pitch) * sin(player.yaw);
    float lookY = sin(player.pitch);
    float lookZ = cos(player.pitch) * cos(player.yaw);
    
    // Normalizar el vector de dirección
    float length = sqrt(lookX * lookX + lookY * lookY + lookZ * lookZ);
    if (length > 0.0f) {
        lookX /= length;
        lookY /= length;
        lookZ /= length;
    }
    
    // Configurar la cámara
    gluLookAt(
        player.x, player.y + player.height, player.z,  // Posición del ojo
        player.x + lookX, player.y + player.height + lookY, player.z + lookZ,  // Punto de mira
        0.0f, 1.0f, 0.0f  // Vector "up" (hacia arriba)
    );
}

void draw_cube(float x, float y, float z, float size) {
    float half = size * 0.5f;
    
    // Asegurar que no hay blending ni transparencia
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    
    // Configurar material sólido para que responda a la iluminación
    GLfloat ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat diffuse[] = {0.7f, 0.7f, 0.7f, 1.0f};
    GLfloat specular[] = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat shininess[] = {32.0f};
    
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

void draw_tall_wall(int x, int z, int levels) {
    // Asegurar que no hay blending ni transparencia
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    
    // Configurar material para paredes (muy brillante)
    GLfloat mat_ambient[] = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat mat_diffuse[] = {1.2f, 1.2f, 1.2f, 1.0f};
    GLfloat mat_specular[] = {0.6f, 0.6f, 0.6f, 1.0f};
    GLfloat mat_shininess[] = {128.0f};
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
    
    // Dibujar una pared alta compuesta por múltiples cubos apilados
    for (int level = 0; level < levels; level++) {
        float y = level + 0.5f; // Altura de cada nivel
        
        // Dibujar cubo sólido
        draw_cube(x, y, z, 1.0f);
    }
}

void draw_floor() {
    // Configurar material para el suelo - color amarillo marrón muy brillante
    GLfloat ambient[] = {0.6f, 0.4f, 0.2f, 1.0f};  // Marrón muy brillante
    GLfloat diffuse[] = {1.2f, 0.8f, 0.4f, 1.0f};   // Amarillo marrón muy visible
    GLfloat specular[] = {0.4f, 0.4f, 0.2f, 1.0f}; // Brillo muy visible
    GLfloat shininess[] = {64.0f};
    
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
    // Configurar material para el techo (más visible)
    GLfloat ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat diffuse[] = {0.6f, 0.6f, 0.6f, 1.0f};
    GLfloat specular[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat shininess[] = {32.0f};
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    
    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f); // Normal hacia abajo
    float ceiling_height = MAZE_LEVELS + 2.0f; // Techo más alto
    glVertex3f(-200.0f, ceiling_height, -200.0f);
    glVertex3f(200.0f, ceiling_height, -200.0f);
    glVertex3f(200.0f, ceiling_height, 200.0f);
    glVertex3f(-200.0f, ceiling_height, 200.0f);
    glEnd();
}

void setup_fog() {
    // Configurar niebla estilo Silent Hill - solo en la distancia y arriba
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP2); // Usar niebla exponencial para efecto más suave
    glFogf(GL_FOG_DENSITY, 0.0005f); // Densidad ultra reducida
    glFogf(GL_FOG_START, 40.0f); // Comenzar la niebla a 40 unidades (mucho más lejos)
    glFogf(GL_FOG_END, 120.0f); // Terminar la niebla a 120 unidades
    
    // Color de la niebla gris claro estilo Silent Hill
    GLfloat fogColor[4] = {0.3f, 0.3f, 0.3f, 1.0f}; // Gris claro
    glFogfv(GL_FOG_COLOR, fogColor);
}

void update_fog_distance() {
    // Actualizar parámetros de niebla
    glFogf(GL_FOG_START, fog_start);
    glFogf(GL_FOG_END, fog_end);
}

void setup_lighting() {
    // Configurar iluminación realista
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1); // Luz adicional para el enemigo
    
    // Configurar luz ambiental (muy brillante para visibilidad)
    GLfloat ambient[] = {0.6f, 0.6f, 0.6f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    
    // Configurar luz difusa (muy brillante para ambiente backrooms)
    GLfloat diffuse[] = {1.5f, 1.5f, 1.2f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    
    // Configurar luz especular (muy visible)
    GLfloat specular[] = {0.8f, 0.8f, 0.8f, 1.0f};
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    
    // Configurar atenuación
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, LIGHT_ATTENUATION);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.1f);
    
    // Configurar rango de la luz
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0f); // Luz omnidireccional
    
    // Configurar material por defecto muy brillante
    GLfloat mat_ambient[] = {0.4f, 0.4f, 0.4f, 1.0f};
    GLfloat mat_diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat mat_specular[] = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat mat_shininess[] = {128.0f};
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

void setup_enemy_lighting(float x, float y, float z) {
    // Configurar luz roja del enemigo más brillante
    GLfloat ambient[] = {0.4f, 0.0f, 0.0f, 1.0f};
    GLfloat diffuse[] = {1.0f, 0.0f, 0.0f, 1.0f};
    GLfloat specular[] = {0.8f, 0.0f, 0.0f, 1.0f};
    GLfloat position[] = {x, y, z, 1.0f}; // Luz puntual
    
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT1, GL_POSITION, position);
    
    // Configurar atenuación para luz del enemigo (más rango)
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.05f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.02f);
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
    
    // Ajustar intensidad de la luz basada en la distancia
    if (distance_from_center > 50.0f) {
        light_intensity = 0.5f; // Menos luz en los bordes
    }
    
    // Actualizar parámetros de niebla
    fog_density = 0.003f * light_intensity;
    fog_start = 15.0f / light_intensity;
    fog_end = 80.0f / light_intensity;
}

void update_lighting() {
    // Actualizar posición de la luz basada en la posición del jugador
    light_x = player.x;
    light_y = player.y + player.height;
    light_z = player.z;
    
    // Configurar posición de la luz
    GLfloat position[] = {light_x, light_y, light_z, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    
    // Actualizar niebla basada en la iluminación
    update_fog_based_on_lighting();
}

void render_world() {
    // Pantalla de carga eliminada
    
    // Limpiar buffers con depth testing
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Asegurar que depth testing esté habilitado
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    
    // Deshabilitar blending para muros sólidos
    glDisable(GL_BLEND);
    
    // Configurar la cámara
    setup_camera();
    
    // Configurar iluminación
    setup_lighting();
    
    // Configurar niebla
    setup_fog();
    
    // Actualizar iluminación dinámica
    update_lighting();
    
    // Actualizar partículas
    update_particles();
    
    // Dibujar suelo y techo
    draw_floor();
    draw_ceiling();
    
    // Renderizar el laberinto 3D con muros completamente sólidos
    // Ordenar por distancia para renderizado suave
    for (int x = 0; x < MAZE_WIDTH; x++) {
        for (int z = 0; z < MAZE_HEIGHT; z++) {
            if (maze[x][z] == 1) { // Si hay una pared
                // Calcular distancia desde el jugador
                float distance = sqrt((x - player.x) * (x - player.x) + 
                                     (z - player.z) * (z - player.z));
                
                // Renderizar muros sólidos con culling más agresivo
                if (distance <= 30.0f) { // Rango fijo más pequeño para mejor rendimiento
                    // Material ya configurado en draw_tall_wall
                    
                    // Asegurar que no hay transparencia
                    glDisable(GL_BLEND);
                    glDepthMask(GL_TRUE);
                    
                    // Dibujar pared con gran altura (8 niveles)
                    draw_tall_wall(x, z, MAZE_LEVELS);
                }
            } else if (maze[x][z] == 2) { // Elementos decorativos
                // Calcular distancia desde el jugador
                float distance = sqrt((x - player.x) * (x - player.x) + 
                                     (z - player.z) * (z - player.z));
                
                // Renderizar elementos decorativos sólidos
                if (distance <= 25.0f) { // Rango fijo más pequeño
                    // Material ya configurado en draw_cube
                    glDisable(GL_BLEND);
                    glDepthMask(GL_TRUE);
                    
                    // Dibujar elemento decorativo más pequeño
                    draw_cube(x, 0.3f, z, 0.2f);
                }
            }
        }
    }
    
    // Renderizar enemigo 3D
    render_enemy_3d();
    
    // Renderizar mini mapa 2D
    render_minimap();
    
    // Renderizar partículas
    render_particles();
}

void render_minimap() {
    // Guardar estado actual de OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();
    
    // Deshabilitar depth testing para el mini mapa
    glDisable(GL_DEPTH_TEST);
    
    // Configurar proyección ortogonal para el mini mapa
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, windowWidth, windowHeight, 0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Dibujar mini mapa en la esquina superior derecha
    draw_minimap_background();
    draw_minimap_walls();
    draw_minimap_player();
    render_enemy_minimap(); // Added for enemy
    
    // Restaurar matrices
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    // Restaurar estado de OpenGL
    glPopMatrix();
    glPopAttrib();
}

void draw_minimap_background() {
    float minimapSize = 200.0f;
    float x = windowWidth - minimapSize - 10;
    float y = 10;
    
    // Habilitar blending para el fondo
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
    
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + minimapSize, y);
    glVertex2f(x + minimapSize, y + minimapSize);
    glVertex2f(x, y + minimapSize);
    glEnd();
    
    glDisable(GL_BLEND);
}

void draw_minimap_walls() {
    float minimapSize = 200.0f;
    float x = windowWidth - minimapSize - 10;
    float y = 10;
    float scale = minimapSize / MAZE_WIDTH; // Escala para el mini mapa
    
    // Asegurar que no hay blending para muros sólidos
    glDisable(GL_BLEND);
    glColor3f(1.0f, 1.0f, 1.0f); // Color de las paredes más brillante
    
    for (int mx = 0; mx < MAZE_WIDTH; mx++) {
        for (int mz = 0; mz < MAZE_HEIGHT; mz++) {
            if (maze[mx][mz] == 1) { // Si hay una pared
                float mapX = x + mx * scale;
                float mapY = y + mz * scale;
                
                glBegin(GL_QUADS);
                glVertex2f(mapX, mapY);
                glVertex2f(mapX + scale, mapY);
                glVertex2f(mapX + scale, mapY + scale);
                glVertex2f(mapX, mapY + scale);
                glEnd();
            }
        }
    }
    
    // Debug removido para mejor rendimiento
}

void draw_minimap_player() {
    float minimapSize = 200.0f;
    float x = windowWidth - minimapSize - 10;
    float y = 10;
    float scale = minimapSize / MAZE_WIDTH;
    
    // Posición del jugador en el mini mapa
    float playerMapX = x + player.x * scale;
    float playerMapY = y + player.z * scale;
    
    // Dibujar jugador como círculo con dirección
    glDisable(GL_BLEND);
    glColor3f(1.0f, 0.0f, 0.0f); // Rojo brillante para el jugador
    
    // Dibujar círculo del jugador más grande
    float radius = scale * 3.0f; // Aumentado de 1.5f a 3.0f
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(playerMapX, playerMapY); // Centro
    for (int i = 0; i <= 32; i++) {
        float angle = 2.0f * M_PI * i / 32.0f;
        glVertex2f(playerMapX + cos(angle) * radius, playerMapY + sin(angle) * radius);
    }
    glEnd();
    
    // Dibujar dirección como línea
    float arrowLength = scale * 2.0f;
    float arrowX = playerMapX + cos(player.yaw) * arrowLength;
    float arrowY = playerMapY + sin(player.yaw) * arrowLength;
    
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glVertex2f(playerMapX, playerMapY);
    glVertex2f(arrowX, arrowY);
    glEnd();
}

void cleanup_renderer() {
    // Limpiar recursos de renderizado
    // Textura de carga eliminada
}
