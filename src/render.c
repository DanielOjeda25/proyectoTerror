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

// Definir M_PI si no está definido
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Variables globales del renderizador (externas)
extern int windowWidth;
extern int windowHeight;

// Variables de niebla
float fog_start = 10.0f;
float fog_end = 50.0f;
float fog_density = 0.015f;

// Variables de iluminación
float light_x = 0.0f;
float light_y = 0.0f;
float light_z = 0.0f;
float light_range = LIGHT_RANGE;

// Variables del sistema de carga eliminadas

// Función para frustum culling basado en perspectiva de cámara
bool is_in_frustum(int x, int z) {
    // Calcular vector desde el jugador al objeto
    float dx = x - player.x;
    float dz = z - player.z;
    
    // Calcular ángulo del objeto relativo a la dirección de la cámara
    float object_angle = atan2(dz, dx);
    float player_angle = player.yaw;
    
    // Normalizar ángulos
    float angle_diff = object_angle - player_angle;
    while (angle_diff > M_PI) angle_diff -= 2.0f * M_PI;
    while (angle_diff < -M_PI) angle_diff += 2.0f * M_PI;
    
    // Campo de visión de 180 grados (90 grados a cada lado) - más generoso para perspectiva
    float fov_half = M_PI / 2.0f;
    
    // Verificar si está dentro del campo de visión
    return fabs(angle_diff) <= fov_half;
}

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
    float lookX = cos(player.pitch) * -sin(player.yaw);
    float lookY = sin(player.pitch);
    float lookZ = cos(player.pitch) * -cos(player.yaw);
    
    // Normalizar el vector de dirección
    float length = sqrt(lookX * lookX + lookY * lookY + lookZ * lookZ);
    if (length > 0.0f) {
        lookX /= length;
        lookY /= length;
        lookZ /= length;
    }
    
    // Configurar la cámara con altura corregida
    float eye_height = player.y + player.height;
    float look_height = eye_height + lookY;
    
    // Asegurar que la cámara esté a una altura mínima
    if (eye_height < 0.5f) eye_height = 0.5f;
    if (look_height < 0.5f) look_height = 0.5f;
    
    gluLookAt(
        player.x, eye_height, player.z,  // Posición del ojo
        player.x + lookX, look_height, player.z + lookZ,  // Punto de mira
        0.0f, 1.0f, 0.0f  // Vector "up" (hacia arriba)
    );
}

void draw_cube(float x, float y, float z, float size) {
    float half = size * 0.5f;
    
    // Asegurar que no hay blending ni transparencia
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    
    // Configurar material sólido para que responda a la iluminación (negro para postes)
    GLfloat ambient[] = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat diffuse[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat specular[] = {0.05f, 0.05f, 0.05f, 1.0f};
    GLfloat shininess[] = {16.0f};
    
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
    
    // Configurar material para paredes (negro oscuro)
    GLfloat mat_ambient[] = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat mat_diffuse[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat mat_specular[] = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat mat_shininess[] = {32.0f};
    
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
    // Configurar material para el suelo - color gris con niveles
    GLfloat ambient[] = {0.4f, 0.4f, 0.4f, 1.0f};  // Gris más claro
    GLfloat diffuse[] = {0.7f, 0.7f, 0.7f, 1.0f};   // Gris medio más visible
    GLfloat specular[] = {0.1f, 0.1f, 0.1f, 1.0f}; // Brillo muy sutil
    GLfloat shininess[] = {16.0f};
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    
    // Dibujar suelo con múltiples niveles para crear verticalidad
    glBegin(GL_QUADS);
    
    // Nivel principal (0.0) - superficie principal
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-200.0f, 0.0f, -200.0f);
    glVertex3f(200.0f, 0.0f, -200.0f);
    glVertex3f(200.0f, 0.0f, 200.0f);
    glVertex3f(-200.0f, 0.0f, 200.0f);
    
    // Nivel secundario (-0.2) - profundidad del terreno
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-200.0f, -0.2f, -200.0f);
    glVertex3f(200.0f, -0.2f, -200.0f);
    glVertex3f(200.0f, -0.2f, 200.0f);
    glVertex3f(-200.0f, -0.2f, 200.0f);
    
    // Nivel terciario (-0.4) - base del terreno
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-200.0f, -0.4f, -200.0f);
    glVertex3f(200.0f, -0.4f, -200.0f);
    glVertex3f(200.0f, -0.4f, 200.0f);
    glVertex3f(-200.0f, -0.4f, 200.0f);
    
    // Paredes laterales para conectar los niveles
    // Pared frontal (Z = -200)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-200.0f, 0.0f, -200.0f);
    glVertex3f(200.0f, 0.0f, -200.0f);
    glVertex3f(200.0f, -0.4f, -200.0f);
    glVertex3f(-200.0f, -0.4f, -200.0f);
    
    // Pared trasera (Z = 200)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-200.0f, 0.0f, 200.0f);
    glVertex3f(-200.0f, -0.4f, 200.0f);
    glVertex3f(200.0f, -0.4f, 200.0f);
    glVertex3f(200.0f, 0.0f, 200.0f);
    
    // Pared izquierda (X = -200)
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-200.0f, 0.0f, -200.0f);
    glVertex3f(-200.0f, -0.4f, -200.0f);
    glVertex3f(-200.0f, -0.4f, 200.0f);
    glVertex3f(-200.0f, 0.0f, 200.0f);
    
    // Pared derecha (X = 200)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(200.0f, 0.0f, -200.0f);
    glVertex3f(200.0f, 0.0f, 200.0f);
    glVertex3f(200.0f, -0.4f, 200.0f);
    glVertex3f(200.0f, -0.4f, -200.0f);
    
    glEnd();
}

void draw_terrain_variations() {
    // Dibujar variaciones en el terreno para crear más verticalidad
    // Configurar material para variaciones del terreno
    GLfloat ambient[] = {0.35f, 0.35f, 0.35f, 1.0f};  // Gris ligeramente diferente
    GLfloat diffuse[] = {0.65f, 0.65f, 0.65f, 1.0f};   // Gris medio
    GLfloat specular[] = {0.05f, 0.05f, 0.05f, 1.0f}; // Brillo muy sutil
    GLfloat shininess[] = {8.0f};
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    
    glBegin(GL_QUADS);
    
    // Crear variaciones en el terreno con diferentes alturas
    // Variación 1: Área central más alta
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-100.0f, 0.1f, -100.0f);
    glVertex3f(100.0f, 0.1f, -100.0f);
    glVertex3f(100.0f, 0.1f, 100.0f);
    glVertex3f(-100.0f, 0.1f, 100.0f);
    
    // Variación 2: Área periférica más baja
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-200.0f, -0.1f, -200.0f);
    glVertex3f(-100.0f, -0.1f, -200.0f);
    glVertex3f(-100.0f, -0.1f, -100.0f);
    glVertex3f(-200.0f, -0.1f, -100.0f);
    
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(100.0f, -0.1f, -200.0f);
    glVertex3f(200.0f, -0.1f, -200.0f);
    glVertex3f(200.0f, -0.1f, -100.0f);
    glVertex3f(100.0f, -0.1f, -100.0f);
    
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-200.0f, -0.1f, 100.0f);
    glVertex3f(-100.0f, -0.1f, 100.0f);
    glVertex3f(-100.0f, -0.1f, 200.0f);
    glVertex3f(-200.0f, -0.1f, 200.0f);
    
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(100.0f, -0.1f, 100.0f);
    glVertex3f(200.0f, -0.1f, 100.0f);
    glVertex3f(200.0f, -0.1f, 200.0f);
    glVertex3f(100.0f, -0.1f, 200.0f);
    
    // Paredes de transición entre niveles
    // Transición de área central a periférica
    glNormal3f(0.0f, 0.0f, -1.0f);
    glVertex3f(-100.0f, 0.1f, -100.0f);
    glVertex3f(100.0f, 0.1f, -100.0f);
    glVertex3f(100.0f, -0.1f, -100.0f);
    glVertex3f(-100.0f, -0.1f, -100.0f);
    
    glNormal3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-100.0f, 0.1f, 100.0f);
    glVertex3f(-100.0f, -0.1f, 100.0f);
    glVertex3f(100.0f, -0.1f, 100.0f);
    glVertex3f(100.0f, 0.1f, 100.0f);
    
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glVertex3f(-100.0f, 0.1f, -100.0f);
    glVertex3f(-100.0f, -0.1f, -100.0f);
    glVertex3f(-100.0f, -0.1f, 100.0f);
    glVertex3f(-100.0f, 0.1f, 100.0f);
    
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(100.0f, 0.1f, -100.0f);
    glVertex3f(100.0f, 0.1f, 100.0f);
    glVertex3f(100.0f, -0.1f, 100.0f);
    glVertex3f(100.0f, -0.1f, -100.0f);
    
    glEnd();
}

float get_terrain_height(float x, float z) {
    // Calcular la altura del terreno basada en la posición
    // Área central elevada (100x100)
    if (x >= -100.0f && x <= 100.0f && z >= -100.0f && z <= 100.0f) {
        return 0.1f; // Área central elevada
    }
    // Área periférica hundida
    else if ((x < -100.0f || x > 100.0f) || (z < -100.0f || z > 100.0f)) {
        return -0.1f; // Área periférica hundida
    }
    // Área de transición (suave)
    else {
        return 0.0f; // Nivel base
    }
}

void draw_ceiling() {
    // Configurar material para el techo (gris)
    GLfloat ambient[] = {0.4f, 0.4f, 0.4f, 1.0f};
    GLfloat diffuse[] = {0.7f, 0.7f, 0.7f, 1.0f};
    GLfloat specular[] = {0.3f, 0.3f, 0.3f, 1.0f};
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
    // Configurar niebla EQUILIBRADA estilo Silent Hill - visible pero atmosférica
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP2); // Usar niebla exponencial
    glFogf(GL_FOG_DENSITY, 0.12f); // Densidad equilibrada - visible pero densa
    glFogf(GL_FOG_START, 5.0f); // Comenzar la niebla CERCA
    glFogf(GL_FOG_END, 25.0f); // Terminar la niebla a distancia media - equilibrada
    
    // Color de la niebla equilibrado y atmosférico
    GLfloat fogColor[4] = {0.15f, 0.15f, 0.18f, 1.0f}; // Gris equilibrado
    glFogfv(GL_FOG_COLOR, fogColor);
}

void update_fog_distance() {
    // Mantener niebla EQUILIBRADA estilo Silent Hill - visible pero atmosférica
    glFogf(GL_FOG_START, 5.0f);
    glFogf(GL_FOG_END, 25.0f);
    glFogf(GL_FOG_DENSITY, 0.12f);
}

void setup_lighting() {
    // Configurar iluminación realista y dinámica
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1); // Luz adicional para el enemigo
    glEnable(GL_LIGHT2); // Luz ambiental adicional
    
    // Configurar luz ambiental (más tenue para ambiente backrooms)
    GLfloat ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    
    // Configurar luz difusa (más realista)
    GLfloat diffuse[] = {0.8f, 0.8f, 0.7f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    
    // Configurar luz especular (más sutil)
    GLfloat specular[] = {0.4f, 0.4f, 0.4f, 1.0f};
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    
    // Configurar atenuación más realista
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.5f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.05f);
    
    // Configurar rango de la luz
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0f); // Luz omnidireccional
    
    // Configurar material por defecto más realista
    GLfloat mat_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat mat_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat mat_specular[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat mat_shininess[] = {64.0f};
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
    
    // Habilitar sombras suaves
    glEnable(GL_NORMALIZE);
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
    // Niebla EQUILIBRADA estilo Silent Hill - visible pero atmosférica
    // No ajustar basado en iluminación para mantener consistencia
    fog_density = 0.12f; // Siempre equilibrada - visible pero densa
    fog_start = 5.0f; // Siempre cerca - equilibrada
    fog_end = 25.0f; // Siempre distancia media - equilibrada
}

void update_lighting() {
    // Actualizar posición de la luz basada en la posición del jugador
    light_x = player.x;
    light_y = player.y + player.height;
    light_z = player.z;
    
    // Configurar posición de la luz principal (linterna del jugador)
    GLfloat position[] = {light_x, light_y, light_z, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    
    // Configurar luz ambiental dinámica basada en la posición
    static float time_counter = 0.0f;
    time_counter += 0.016f; // Aproximadamente 60 FPS
    float ambient_intensity = 0.2f + 0.1f * sin(time_counter * 0.5f);
    GLfloat ambient[] = {ambient_intensity, ambient_intensity, ambient_intensity * 0.9f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    
    // Configurar luz ambiental adicional (LIGHT2) para iluminación general
    GLfloat ambient2[] = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat diffuse2[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat position2[] = {0.0f, 10.0f, 0.0f, 1.0f}; // Luz desde arriba
    
    glLightfv(GL_LIGHT2, GL_AMBIENT, ambient2);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse2);
    glLightfv(GL_LIGHT2, GL_POSITION, position2);
    
    // Actualizar niebla basada en la iluminación
    update_fog_based_on_lighting();
}

void render_world() {
    // Verificar que el mapa esté precargado
    if (!is_map_ready()) {
        render_map_loading_screen();
        return;
    }
    
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
    
    // Actualizar niebla dinámicamente
    update_fog_distance();
    
    // Forzar configuración de niebla EQUILIBRADA estilo Silent Hill en cada frame
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP2);
    glFogf(GL_FOG_DENSITY, 0.12f); // Densidad equilibrada - visible pero densa
    glFogf(GL_FOG_START, 5.0f); // CERCA
    glFogf(GL_FOG_END, 25.0f); // DISTANCIA MEDIA - equilibrada
    GLfloat fogColor[4] = {0.15f, 0.15f, 0.18f, 1.0f}; // Equilibrado
    glFogfv(GL_FOG_COLOR, fogColor);
    
    // Actualizar iluminación dinámica (cada 3 frames para mejor rendimiento)
    static int lighting_frame_counter = 0;
    if (lighting_frame_counter % 3 == 0) {
        update_lighting();
    }
    lighting_frame_counter++;
    
    // Actualizar partículas
    update_particles();
    
    // Dibujar suelo y techo DESPUÉS de configurar la niebla
    // (para que estén afectados por la niebla densa)
    draw_floor();
    draw_terrain_variations(); // Añadir variaciones del terreno
    draw_ceiling();
    
    // Renderizar el laberinto 3D basado en la PERSPECTIVA DE LA CÁMARA
    // Calcular rango de renderizado basado en la dirección de la cámara
    float render_distance = 35.0f; // Distancia aumentada para área de carga más grande
    
    // Optimización: usar distancia al cuadrado para evitar sqrt costoso
    float render_distance_sq = render_distance * render_distance;
    
    // Sistema basado en perspectiva de cámara: área más grande en la dirección de la mirada
    int start_x = (int)(player.x - render_distance);
    int end_x = (int)(player.x + render_distance);
    int start_z = (int)(player.z - render_distance);
    int end_z = (int)(player.z + render_distance);
    
    // Limitar a los bordes del mapa
    if (start_x < 0) start_x = 0;
    if (end_x >= MAZE_WIDTH) end_x = MAZE_WIDTH - 1;
    if (start_z < 0) start_z = 0;
    if (end_z >= MAZE_HEIGHT) end_z = MAZE_HEIGHT - 1;
    
    // Renderizar basado en la perspectiva de la cámara
    // Priorizar objetos en la dirección de la mirada del jugador
    for (int x = start_x; x <= end_x; x++) {
        for (int z = start_z; z <= end_z; z++) {
            if (maze[x][z] == 1) { // Si hay una pared
                // Calcular distancia al cuadrado (más eficiente que sqrt)
                float dx = x - player.x;
                float dz = z - player.z;
                float distance_sq = dx * dx + dz * dz;
                
                // Verificar distancia primero (más rápido)
                if (distance_sq <= render_distance_sq) {
                    // Solo calcular sqrt cuando sea necesario
                    float distance = sqrt(distance_sq);
                    
                    // Sistema de prioridades: objetos cercanos siempre se renderizan
                    bool should_render = false;
                    int levels = MAZE_LEVELS;
                    
                    if (distance <= 25.0f) {
                        // Zona cercana: siempre renderizar (área de carga aumentada)
                        should_render = true;
                        levels = MAZE_LEVELS;
                    } else if (distance <= 35.0f) {
                        // Zona media: renderizar con frustum culling basado en perspectiva
                        should_render = is_in_frustum(x, z);
                        levels = MAZE_LEVELS;
                    } else {
                        // Zona lejana: NO RENDERIZAR (oculto por niebla)
                        should_render = false;
                        levels = 0;
                    }
                    
                    if (should_render) {
                        // Material ya configurado en draw_tall_wall
                        glDisable(GL_BLEND);
                        glDepthMask(GL_TRUE);
                        
                        // Dibujar pared con altura optimizada
                        draw_tall_wall(x, z, levels);
                    }
                }
            } else if (maze[x][z] == 2) { // Elementos decorativos
                // Calcular distancia al cuadrado (más eficiente)
                float dx = x - player.x;
                float dz = z - player.z;
                float distance_sq = dx * dx + dz * dz;
                float decor_distance_sq = 35.0f * 35.0f;
                
                // Sistema de prioridades para elementos decorativos
                if (distance_sq <= decor_distance_sq) {
                    float distance = sqrt(distance_sq);
                    bool should_render = false;
                    float size = 0.2f;
                    
                    if (distance <= 20.0f) {
                        // Zona cercana: siempre renderizar (área de carga aumentada)
                        should_render = true;
                        size = 0.2f;
                    } else if (distance <= 35.0f) {
                        // Zona media: con frustum culling basado en perspectiva
                        should_render = is_in_frustum(x, z);
                        size = 0.2f;
                    } else {
                        // Zona lejana: NO RENDERIZAR (oculto por niebla)
                        should_render = false;
                        size = 0.0f;
                    }
                    
                    if (should_render) {
                        glDisable(GL_BLEND);
                        glDepthMask(GL_TRUE);
                        
                        // Dibujar elemento decorativo optimizado
                        draw_cube(x, 0.3f, z, size);
                    }
                }
            }
        }
    }
    
    // Renderizar enemigo 3D
    render_enemy_3d();
    
    // Mini mapa eliminado para mejor rendimiento
    
    // Renderizar partículas
    render_particles();
}

// Todas las funciones del minimapa eliminadas para mejor rendimiento

void cleanup_renderer() {
    // Limpiar recursos del renderizador
    // Funciones del minimapa eliminadas para mejor rendimiento
}

void render_map_loading_screen() {
    // Pantalla de carga para la precarga del mapa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Configurar proyección ortogonal para texto
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, windowWidth, windowHeight, 0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Deshabilitar depth testing para la pantalla de carga
    glDisable(GL_DEPTH_TEST);
    
    // Fondo negro
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(windowWidth, 0);
    glVertex2f(windowWidth, windowHeight);
    glVertex2f(0, windowHeight);
    glEnd();
    
    // Texto de carga (simulado con rectángulos)
    glColor3f(1.0f, 1.0f, 1.0f);
    
    // Título
    float titleX = windowWidth / 2.0f - 200.0f;
    float titleY = windowHeight / 2.0f - 100.0f;
    
    // Dibujar "PRECARGANDO MAPA" como rectángulos
    glBegin(GL_QUADS);
    // P
    glVertex2f(titleX, titleY);
    glVertex2f(titleX + 20, titleY);
    glVertex2f(titleX + 20, titleY + 40);
    glVertex2f(titleX, titleY + 40);
    glVertex2f(titleX, titleY + 20);
    glVertex2f(titleX + 15, titleY + 20);
    glVertex2f(titleX + 15, titleY + 10);
    glVertex2f(titleX, titleY + 10);
    // R
    glVertex2f(titleX + 30, titleY);
    glVertex2f(titleX + 50, titleY);
    glVertex2f(titleX + 50, titleY + 40);
    glVertex2f(titleX + 30, titleY + 40);
    // E
    glVertex2f(titleX + 60, titleY);
    glVertex2f(titleX + 80, titleY);
    glVertex2f(titleX + 80, titleY + 40);
    glVertex2f(titleX + 60, titleY + 40);
    // C
    glVertex2f(titleX + 90, titleY);
    glVertex2f(titleX + 110, titleY);
    glVertex2f(titleX + 110, titleY + 40);
    glVertex2f(titleX + 90, titleY + 40);
    glEnd();
    
    // Barra de progreso
    float progressBarX = windowWidth / 2.0f - 150.0f;
    float progressBarY = windowHeight / 2.0f + 50.0f;
    float progressBarWidth = 300.0f;
    float progressBarHeight = 20.0f;
    
    // Fondo de la barra
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(progressBarX, progressBarY);
    glVertex2f(progressBarX + progressBarWidth, progressBarY);
    glVertex2f(progressBarX + progressBarWidth, progressBarY + progressBarHeight);
    glVertex2f(progressBarX, progressBarY + progressBarHeight);
    glEnd();
    
    // Barra de progreso (animada)
    static float progress = 0.0f;
    progress += 0.5f;
    if (progress > progressBarWidth) progress = 0.0f;
    
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(progressBarX, progressBarY);
    glVertex2f(progressBarX + progress, progressBarY);
    glVertex2f(progressBarX + progress, progressBarY + progressBarHeight);
    glVertex2f(progressBarX, progressBarY + progressBarHeight);
    glEnd();
    
    // Restaurar matrices
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    
    // Rehabilitar depth testing
    glEnable(GL_DEPTH_TEST);
}

// Función cleanup_renderer ya definida anteriormente
