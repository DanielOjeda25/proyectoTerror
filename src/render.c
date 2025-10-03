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

// Función para frustum culling básico
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
    
    // Campo de visión de 150 grados (75 grados a cada lado) - más generoso
    float fov_half = M_PI * 5.0f / 12.0f;
    
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
    // Configurar niebla MUY densa para ocultar completamente el fondo
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP2); // Usar niebla exponencial
    glFogf(GL_FOG_DENSITY, 0.05f); // Densidad MUY alta
    glFogf(GL_FOG_START, 5.0f); // Comenzar la niebla muy cerca
    glFogf(GL_FOG_END, 30.0f); // Terminar la niebla más cerca
    
    // Color de la niebla muy denso y oscuro
    GLfloat fogColor[4] = {0.1f, 0.1f, 0.15f, 1.0f}; // Gris muy oscuro
    glFogfv(GL_FOG_COLOR, fogColor);
}

void update_fog_distance() {
    // Mantener niebla MUY densa fija para ocultar completamente el fondo
    glFogf(GL_FOG_START, 5.0f);
    glFogf(GL_FOG_END, 30.0f);
    glFogf(GL_FOG_DENSITY, 0.05f);
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
    
    // Actualizar niebla dinámicamente
    update_fog_distance();
    
    // Forzar configuración de niebla en cada frame
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP2);
    glFogf(GL_FOG_DENSITY, 0.05f);
    glFogf(GL_FOG_START, 5.0f);
    glFogf(GL_FOG_END, 30.0f);
    GLfloat fogColor[4] = {0.1f, 0.1f, 0.15f, 1.0f};
    glFogfv(GL_FOG_COLOR, fogColor);
    
    // Actualizar iluminación dinámica (cada 3 frames para mejor rendimiento)
    static int lighting_frame_counter = 0;
    if (lighting_frame_counter % 3 == 0) {
        update_lighting();
    }
    lighting_frame_counter++;
    
    // Actualizar partículas
    update_particles();
    
    // Dibujar suelo y techo
    draw_floor();
    draw_ceiling();
    
    // Renderizar el laberinto 3D con carga progresiva inteligente
    // Calcular rango de renderizado basado en la dirección de la cámara
    float render_distance = 25.0f; // Distancia reducida para que los muros aparezcan más cerca
    float player_yaw = player.yaw;
    
    // Optimización: usar distancia al cuadrado para evitar sqrt costoso
    float render_distance_sq = render_distance * render_distance;
    
    // Sistema de carga progresiva: área más grande pero con prioridades
    int start_x = (int)(player.x - render_distance);
    int end_x = (int)(player.x + render_distance);
    int start_z = (int)(player.z - render_distance);
    int end_z = (int)(player.z + render_distance);
    
    // Limitar a los bordes del mapa
    if (start_x < 0) start_x = 0;
    if (end_x >= MAZE_WIDTH) end_x = MAZE_WIDTH - 1;
    if (start_z < 0) start_z = 0;
    if (end_z >= MAZE_HEIGHT) end_z = MAZE_HEIGHT - 1;
    
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
                    
                    if (distance <= 15.0f) {
                        // Zona cercana: siempre renderizar con máximo detalle
                        should_render = true;
                        levels = MAZE_LEVELS;
                    } else if (distance <= 25.0f) {
                        // Zona media: renderizar con frustum culling más permisivo
                        should_render = is_in_frustum(x, z);
                        levels = MAZE_LEVELS;
                    } else {
                        // Zona lejana: solo si está en el campo de visión y con menos detalle
                        should_render = is_in_frustum(x, z);
                        levels = MAZE_LEVELS / 2; // Reducir altura para objetos lejanos
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
                float decor_distance_sq = 20.0f * 20.0f;
                
                // Sistema de prioridades para elementos decorativos
                if (distance_sq <= decor_distance_sq) {
                    float distance = sqrt(distance_sq);
                    bool should_render = false;
                    float size = 0.2f;
                    
                    if (distance <= 12.0f) {
                        // Zona cercana: siempre renderizar
                        should_render = true;
                        size = 0.2f;
                    } else if (distance <= 20.0f) {
                        // Zona media: con frustum culling permisivo
                        should_render = is_in_frustum(x, z);
                        size = 0.2f;
                    } else {
                        // Zona lejana: solo si está visible y más pequeño
                        should_render = is_in_frustum(x, z);
                        size = 0.1f;
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
    
    // Dibujar fondo del mini mapa
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + minimapSize, y);
    glVertex2f(x + minimapSize, y + minimapSize);
    glVertex2f(x, y + minimapSize);
    glEnd();
    
    // Dibujar borde del mini mapa
    glDisable(GL_BLEND);
    glColor3f(0.8f, 0.8f, 0.8f); // Color del borde
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + minimapSize, y);
    glVertex2f(x + minimapSize, y + minimapSize);
    glVertex2f(x, y + minimapSize);
    glEnd();
    glLineWidth(1.0f);
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
