// render.c - Sistema de renderizado 3D para Backrooms
#include "render.h"
#include "player.h"
#include "map.h"
#include "image_loader.h"
#include "particles.h"
#include "enemy.h"
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

// Variables externas
extern Player3D player;
extern int maze[MAZE_WIDTH][MAZE_HEIGHT];
extern LightPoint lightPoints[50];
extern int lightCount;
extern int windowWidth;
extern int windowHeight;

// Variables de iluminación
float light_x = 0.0f;
float light_y = 0.0f;
float light_z = 0.0f;
float light_range = LIGHT_RANGE;

// Variables del sistema de carga
GLuint loading_texture = 0;
bool show_loading_screen = true;
float loading_fade_alpha = 1.0f;
clock_t loading_start_time = 0;

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
    
    // Cargar textura de carga
    load_loading_texture();
    
    // Inicializar sistema de partículas
    init_particles();
    
    // Inicializar tiempo para efectos dinámicos
    loading_start_time = clock();
}

void setup_camera() {
    glLoadIdentity();
    
    // Calcular posición de la cámara
    float camX = player.x;
    float camY = player.y + player.height;
    float camZ = player.z;
    
    // Calcular dirección de la cámara basada en yaw y pitch
    // Sistema estándar: yaw=0 mira hacia X positivo (adelante)
    float lookX = cos(player.yaw) * cos(player.pitch);
    float lookY = sin(player.pitch);
    float lookZ = sin(player.yaw) * cos(player.pitch);
    
    // Normalizar el vector de dirección para evitar drift
    float length = sqrt(lookX * lookX + lookY * lookY + lookZ * lookZ);
    if (length > 0.0f) {
        lookX /= length;
        lookY /= length;
        lookZ /= length;
    }
    
    // Calcular vector "arriba" que rota con la cámara para evitar inclinación
    // El vector arriba siempre apunta hacia Y positivo, independientemente de la rotación
    float upX = 0.0f;
    float upY = 1.0f;
    float upZ = 0.0f;
    
    // Configurar la cámara con vector arriba fijo para evitar drift
    gluLookAt(camX, camY, camZ,                    // Posición de la cámara
              camX + lookX, camY + lookY, camZ + lookZ,  // Punto al que mira
              upX, upY, upZ);                      // Vector "arriba" fijo
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
    GLfloat shininess[] = {10.0f};
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    
    // Asegurar color sólido
    glColor3f(1.0f, 1.0f, 1.0f);
    
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
    
    // Dibujar una pared alta compuesta por múltiples cubos apilados
    for (int level = 0; level < levels; level++) {
        float y = level + 0.5f; // Altura de cada nivel
        
        // Configurar material sólido para cada cubo
        glColor3f(1.0f, 1.0f, 1.0f);
        
        // Dibujar cubo sólido
        draw_cube(x, y, z, 1.0f);
    }
}

void draw_floor() {
    // Configurar material para el suelo - color amarillo marrón
    GLfloat ambient[] = {0.2f, 0.15f, 0.1f, 1.0f};  // Marrón oscuro
    GLfloat diffuse[] = {0.6f, 0.4f, 0.2f, 1.0f};   // Amarillo marrón
    GLfloat specular[] = {0.1f, 0.1f, 0.05f, 1.0f}; // Brillo tenue
    GLfloat shininess[] = {10.0f};
    
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
    // Configurar material para el techo (más alto para que sea menos visible)
    GLfloat ambient[] = {0.02f, 0.02f, 0.02f, 1.0f};
    GLfloat diffuse[] = {0.1f, 0.1f, 0.1f, 1.0f};
    GLfloat specular[] = {0.01f, 0.01f, 0.01f, 1.0f};
    GLfloat shininess[] = {1.0f};
    
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    
    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f); // Normal hacia abajo
    float ceiling_height = MAZE_LEVELS + 2.0f; // Techo más alto
    glVertex3f(-200.0f, ceiling_height, -200.0f);
    glVertex3f(-200.0f, ceiling_height, 200.0f);
    glVertex3f(200.0f, ceiling_height, 200.0f);
    glVertex3f(200.0f, ceiling_height, -200.0f);
    glEnd();
}

void render_world() {
    // Verificar si mostrar pantalla de carga
    if (show_loading_screen) {
        render_loading_screen();
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
                    // Configurar material sólido
                    glColor3f(1.0f, 1.0f, 1.0f);
                    
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
                    // Elementos decorativos siempre sólidos
                    glColor3f(1.0f, 1.0f, 1.0f);
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

void setup_fog() {
    // Configurar niebla para crear ambiente de backrooms infinito
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP2); // Usar niebla exponencial para efecto más suave
    glFogf(GL_FOG_DENSITY, 0.008f); // Densidad reducida para mejor visibilidad
    glFogf(GL_FOG_START, 8.0f); // Comenzar la niebla a 8 unidades
    glFogf(GL_FOG_END, 60.0f); // Terminar la niebla a 60 unidades
    
    // Color de la niebla más claro
    GLfloat fogColor[4] = {0.15f, 0.15f, 0.12f, 1.0f}; // Más claro
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


// ===== SISTEMA DE MINI MAPA 2D =====

void render_minimap() {
    // Guardar estado actual de OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();
    
    // Deshabilitar iluminación y niebla para el mini mapa
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
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
    render_enemy_minimap();
    
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
    // Fondo del mini mapa (esquina superior derecha)
    float minimapSize = 200.0f;
    float x = windowWidth - minimapSize - 10;
    float y = 10;
    
    // Habilitar blending para el fondo semi-transparente
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.8f); // Fondo más opaco
    
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + minimapSize, y);
    glVertex2f(x + minimapSize, y + minimapSize);
    glVertex2f(x, y + minimapSize);
    glEnd();
    
    // Borde del mini mapa
    glDisable(GL_BLEND);
    glColor3f(0.7f, 0.7f, 0.7f); // Borde más visible
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + minimapSize, y);
    glVertex2f(x + minimapSize, y + minimapSize);
    glVertex2f(x, y + minimapSize);
    glEnd();
}

void draw_minimap_walls() {
    float minimapSize = 200.0f;
    float x = windowWidth - minimapSize - 10;
    float y = 10;
    float scale = minimapSize / MAZE_WIDTH; // Escala para el mini mapa
    
    // Asegurar que no hay blending para muros sólidos
    glDisable(GL_BLEND);
    glColor3f(1.0f, 1.0f, 1.0f); // Color de las paredes más brillante
    
    int wall_count = 0;
    for (int mx = 0; mx < MAZE_WIDTH; mx++) {
        for (int mz = 0; mz < MAZE_HEIGHT; mz++) {
            if (maze[mx][mz] == 1) { // Si hay una pared
                wall_count++;
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
    
    // Dibujar círculo del jugador
    float radius = scale * 1.5f;
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


// ===== SISTEMA DE PANTALLA DE CARGA =====

void load_loading_texture() {
    // Intentar cargar la imagen BMP
    ImageData* image = load_bmp("image.bmp");
    
    if (image) {
        // Crear textura desde la imagen BMP
        loading_texture = create_texture_from_image(image);
        free_image(image);
        printf("Imagen de carga cargada exitosamente\n");
    } else {
        // Fallback: crear textura placeholder
        glGenTextures(1, &loading_texture);
        glBindTexture(GL_TEXTURE_2D, loading_texture);
        
        // Configurar parámetros de textura
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        
        // Crear una textura simple de color sólido como placeholder
        unsigned char texture_data[4] = {255, 255, 255, 255}; // Blanco
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);
        printf("Usando textura placeholder (image.bmp no encontrada)\n");
    }
}

void render_loading_screen() {
    // Calcular tiempo transcurrido
    clock_t current_time = clock();
    double elapsed_seconds = ((double)(current_time - loading_start_time)) / CLOCKS_PER_SEC;
    
    // Deshabilitar iluminación y niebla para la pantalla de carga
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glDisable(GL_DEPTH_TEST);
    
    // Configurar proyección ortogonal para pantalla completa
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, windowWidth, windowHeight, 0, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Dibujar fondo gris oscuro en lugar de negro
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(windowWidth, 0);
    glVertex2f(windowWidth, windowHeight);
    glVertex2f(0, windowHeight);
    glEnd();
    
    // Dibujar imagen de carga
    float imageWidth = 400.0f;
    float imageHeight = 300.0f;
    float imageX = (windowWidth - imageWidth) / 2;
    float imageY = (windowHeight - imageHeight) / 2;
    
    // Habilitar texturas
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, loading_texture);
    
    // Dibujar imagen con textura
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(imageX, imageY);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(imageX + imageWidth, imageY);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(imageX + imageWidth, imageY + imageHeight);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(imageX, imageY + imageHeight);
    glEnd();
    
    // Dibujar borde negro
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(imageX, imageY);
    glVertex2f(imageX + imageWidth, imageY);
    glVertex2f(imageX + imageWidth, imageY + imageHeight);
    glVertex2f(imageX, imageY + imageHeight);
    glEnd();
    
    // Dibujar texto "CARGANDO..." en la parte inferior
    glColor3f(1.0f, 1.0f, 1.0f);
    float textY = imageY + imageHeight + 20;
    float textX = (windowWidth - 200) / 2;
    
    // Dibujar fondo para el texto
    glBegin(GL_QUADS);
    glVertex2f(textX - 10, textY - 10);
    glVertex2f(textX + 200, textY - 10);
    glVertex2f(textX + 200, textY + 30);
    glVertex2f(textX - 10, textY + 30);
    glEnd();
    
    // Dibujar texto simple (rectángulos que forman "CARGANDO...")
    glColor3f(0.0f, 0.0f, 0.0f);
    float charWidth = 15;
    float charHeight = 20;
    
    // Dibujar "CARGANDO" con rectángulos simples
    for (int i = 0; i < 9; i++) {
        float x = textX + i * (charWidth + 5);
        glBegin(GL_QUADS);
        glVertex2f(x, textY);
        glVertex2f(x + charWidth, textY);
        glVertex2f(x + charWidth, textY + charHeight);
        glVertex2f(x, textY + charHeight);
        glEnd();
    }
    
    // Después de 2 segundos, comenzar fundido
    if (elapsed_seconds >= 2.0) {
        // Calcular alpha para fundido (2 segundos de fundido)
        float fade_duration = 2.0f;
        float fade_progress = (elapsed_seconds - 2.0) / fade_duration;
        
        if (fade_progress >= 1.0f) {
            // Fundido completo, ocultar pantalla de carga
            show_loading_screen = false;
        } else {
            // Aplicar fundido negro
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(0.0f, 0.0f, 0.0f, fade_progress);
            
            glBegin(GL_QUADS);
            glVertex2f(0, 0);
            glVertex2f(windowWidth, 0);
            glVertex2f(windowWidth, windowHeight);
            glVertex2f(0, windowHeight);
            glEnd();
            
            glDisable(GL_BLEND);
        }
    }
    
    // Restaurar matrices
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}


void cleanup_renderer() {
    // Limpiar recursos de renderizado
    if (loading_texture != 0) {
        glDeleteTextures(1, &loading_texture);
        loading_texture = 0;
    }
}
