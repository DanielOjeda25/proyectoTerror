// render.c - Sistema de renderizado 3D para Backrooms
#include "render.h"
#include "player.h"
#include "map.h"
#include "image_loader.h"
#include "particles.h"
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

void draw_tall_wall(int x, int z, int levels) {
    // Dibujar una pared alta compuesta por múltiples cubos apilados
    for (int level = 0; level < levels; level++) {
        float y = level + 0.5f; // Altura de cada nivel
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
    
    // Actualizar iluminación dinámica
    update_lighting();
    
    // Actualizar partículas
    update_particles();
    
    // Configurar la cámara
    setup_camera();
    
    // Dibujar suelo y techo
    draw_floor();
    draw_ceiling();
    
    // Renderizar el laberinto 3D con carga progresiva suave basada en la luz
    for (int x = 0; x < MAZE_WIDTH; x++) {
        for (int z = 0; z < MAZE_HEIGHT; z++) {
            if (maze[x][z] == 1) { // Si hay una pared
                // Calcular distancia desde el jugador
                float distance = sqrt((x - player.x) * (x - player.x) + 
                                     (z - player.z) * (z - player.z));
                
                // Renderizar muros sólidos sin transparencia
                if (distance <= light_range * 1.2f) { // Rango de renderizado
                    // Muros siempre sólidos - sin transparencia
                    glColor3f(1.0f, 1.0f, 1.0f);
                    
                    // Dibujar pared con gran altura (8 niveles)
                    draw_tall_wall(x, z, MAZE_LEVELS);
                }
            } else if (maze[x][z] == 2) { // Elementos decorativos
                // Calcular distancia desde el jugador
                float distance = sqrt((x - player.x) * (x - player.x) + 
                                     (z - player.z) * (z - player.z));
                
                // Renderizar con transición suave
                if (distance <= light_range * 1.2f) { // Rango extendido
                    // Calcular opacidad basada en distancia
                    float fadeFactor = 1.0f - (distance / (light_range * 1.2f));
                    fadeFactor = (fadeFactor < 0.0f) ? 0.0f : fadeFactor;
                    
                    // Aplicar transparencia suave
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                    glColor4f(1.0f, 1.0f, 1.0f, fadeFactor);
                    
                    // Dibujar elemento decorativo más pequeño
                    draw_cube(x, 0.3f, z, 0.2f);
                    
                    glDisable(GL_BLEND);
                }
            }
        }
    }
    
    // Renderizar puntos de luz como guías
    render_light_points();
    
    // Renderizar mini mapa 2D
    render_minimap();
    
    // Renderizar partículas
    render_particles();
}

void setup_fog() {
    // Configurar niebla para crear ambiente de backrooms infinito
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP2); // Usar niebla exponencial para efecto más suave
    glFogf(GL_FOG_DENSITY, 0.008f); // Densidad reducida para menos oscuridad
    glFogf(GL_FOG_START, 0.0f);
    glFogf(GL_FOG_END, 1.0f);
    
    // Color de la niebla menos denso para mejor visibilidad
    GLfloat fogColor[4] = {0.05f, 0.05f, 0.03f, 1.0f}; // Menos oscuro para mejor visibilidad
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
    
    // Configurar luces adicionales de los puntos de luz
    setup_additional_lights();
    
    // Actualizar niebla basada en la iluminación
    update_fog_based_on_lighting();
}

void render_light_points() {
    // Deshabilitar iluminación para los puntos de luz
    glDisable(GL_LIGHTING);
    
    // Renderizar cada punto de luz
    for (int i = 0; i < lightCount; i++) {
        if (!lightPoints[i].active) continue;
        
        // Calcular distancia desde el jugador
        float distance = sqrt((lightPoints[i].x - player.x) * (lightPoints[i].x - player.x) + 
                             (lightPoints[i].z - player.z) * (lightPoints[i].z - player.z));
        
        // Solo renderizar si está dentro del rango de visión
        if (distance <= lightPoints[i].range) {
            // Configurar color basado en el tipo de luz
            switch (lightPoints[i].type) {
                case 0: // Luz tenue - azul tenue
                    glColor3f(0.2f, 0.3f, 0.8f);
                    break;
                case 1: // Luz normal - amarilla
                    glColor3f(0.9f, 0.9f, 0.6f);
                    break;
                case 2: // Luz brillante - blanca
                    glColor3f(1.0f, 1.0f, 0.9f);
                    break;
            }
            
            // Aplicar intensidad
            GLfloat currentColor[4];
            glGetFloatv(GL_CURRENT_COLOR, currentColor);
            glColor3f(currentColor[0] * lightPoints[i].intensity,
                     currentColor[1] * lightPoints[i].intensity,
                     currentColor[2] * lightPoints[i].intensity);
            
            // Dibujar esfera de luz
            glPushMatrix();
            glTranslatef(lightPoints[i].x, 1.5f, lightPoints[i].z);
            
            // Dibujar esfera simple usando GLU
            GLUquadric* quad = gluNewQuadric();
            gluSphere(quad, 0.3f, 8, 8);
            gluDeleteQuadric(quad);
            
            glPopMatrix();
            
            // Dibujar halo de luz en el suelo
            glPushMatrix();
            glTranslatef(lightPoints[i].x, 0.01f, lightPoints[i].z);
            glScalef(2.0f, 1.0f, 2.0f);
            
            glBegin(GL_QUADS);
            glColor4f(currentColor[0] * 0.3f,
                     currentColor[1] * 0.3f,
                     currentColor[2] * 0.3f, 0.5f);
            glVertex3f(-1.0f, 0.0f, -1.0f);
            glVertex3f(1.0f, 0.0f, -1.0f);
            glVertex3f(1.0f, 0.0f, 1.0f);
            glVertex3f(-1.0f, 0.0f, 1.0f);
            glEnd();
            
            glPopMatrix();
        }
    }
    
    // Rehabilitar iluminación
    glEnable(GL_LIGHTING);
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
    draw_minimap_lights();
    draw_minimap_player();
    
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
    
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f); // Fondo semi-transparente
    
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + minimapSize, y);
    glVertex2f(x + minimapSize, y + minimapSize);
    glVertex2f(x, y + minimapSize);
    glEnd();
    
    // Borde del mini mapa
    glColor3f(0.5f, 0.5f, 0.5f);
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
    
    glColor3f(0.8f, 0.8f, 0.8f); // Color de las paredes
    
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
}

void draw_minimap_player() {
    float minimapSize = 200.0f;
    float x = windowWidth - minimapSize - 10;
    float y = 10;
    float scale = minimapSize / MAZE_WIDTH;
    
    // Posición del jugador en el mini mapa
    float playerMapX = x + player.x * scale;
    float playerMapY = y + player.z * scale;
    
    // Dibujar jugador como una flecha que apunta en la dirección de movimiento
    glColor3f(1.0f, 0.0f, 0.0f); // Rojo para el jugador
    
    glPushMatrix();
    glTranslatef(playerMapX, playerMapY, 0);
    glRotatef(player.yaw * 180.0f / M_PI, 0, 0, 1); // Rotar según la dirección
    
    // Dibujar flecha más clara y visible
    float arrowSize = scale * 1.5f; // Hacer la flecha un poco más grande
    
    glBegin(GL_TRIANGLES);
    // Punta de la flecha (más larga y puntiaguda)
    glVertex2f(0, -arrowSize * 2.5f); // Punta principal
    glVertex2f(-arrowSize * 0.6f, -arrowSize * 0.8f); // Esquina izquierda
    glVertex2f(arrowSize * 0.6f, -arrowSize * 0.8f); // Esquina derecha
    glEnd();
    
    // Cuerpo de la flecha (rectángulo)
    glBegin(GL_QUADS);
    glVertex2f(-arrowSize * 0.3f, -arrowSize * 0.8f);
    glVertex2f(arrowSize * 0.3f, -arrowSize * 0.8f);
    glVertex2f(arrowSize * 0.3f, arrowSize * 0.8f);
    glVertex2f(-arrowSize * 0.3f, arrowSize * 0.8f);
    glEnd();
    
    // Cola de la flecha (para mayor claridad)
    glBegin(GL_TRIANGLES);
    glVertex2f(-arrowSize * 0.3f, arrowSize * 0.8f);
    glVertex2f(0, arrowSize * 1.2f);
    glVertex2f(arrowSize * 0.3f, arrowSize * 0.8f);
    glEnd();
    
    glPopMatrix();
}

void draw_minimap_lights() {
    float minimapSize = 200.0f;
    float x = windowWidth - minimapSize - 10;
    float y = 10;
    float scale = minimapSize / MAZE_WIDTH;
    
    // Dibujar puntos de luz en el mini mapa
    for (int i = 0; i < lightCount; i++) {
        if (!lightPoints[i].active) continue;
        
        float lightMapX = x + lightPoints[i].x * scale;
        float lightMapY = y + lightPoints[i].z * scale;
        
        // Color basado en el tipo de luz
        switch (lightPoints[i].type) {
            case 0: // Luz tenue - azul
                glColor3f(0.2f, 0.3f, 0.8f);
                break;
            case 1: // Luz normal - amarilla
                glColor3f(0.9f, 0.9f, 0.6f);
                break;
            case 2: // Luz brillante - blanca
                glColor3f(1.0f, 1.0f, 0.9f);
                break;
        }
        
        // Dibujar punto de luz
        glPointSize(4.0f);
        glBegin(GL_POINTS);
        glVertex2f(lightMapX, lightMapY);
        glEnd();
    }
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

void setup_additional_lights() {
    // Configurar luces adicionales para los puntos de luz
    int light_index = 1; // Empezar desde GL_LIGHT1
    
    for (int i = 0; i < lightCount && light_index < 8; i++) {
        if (!lightPoints[i].active) continue;
        
        // Calcular distancia desde el jugador
        float distance = sqrt((lightPoints[i].x - player.x) * (lightPoints[i].x - player.x) + 
                             (lightPoints[i].z - player.z) * (lightPoints[i].z - player.z));
        
        // Solo configurar luces cercanas
        if (distance <= lightPoints[i].range * 2.0f) {
            glEnable(GL_LIGHT0 + light_index);
            
            // Configurar posición de la luz
            GLfloat position[] = {lightPoints[i].x, 1.5f, lightPoints[i].z, 1.0f};
            glLightfv(GL_LIGHT0 + light_index, GL_POSITION, position);
            
            // Configurar colores basados en el tipo de luz
            GLfloat ambient[4], diffuse[4], specular[4];
            
            switch (lightPoints[i].type) {
                case 0: // Luz tenue - azul
                    ambient[0] = 0.1f; ambient[1] = 0.1f; ambient[2] = 0.3f; ambient[3] = 1.0f;
                    diffuse[0] = 0.2f; diffuse[1] = 0.3f; diffuse[2] = 0.8f; diffuse[3] = 1.0f;
                    specular[0] = 0.1f; specular[1] = 0.1f; specular[2] = 0.4f; specular[3] = 1.0f;
                    break;
                case 1: // Luz normal - amarilla
                    ambient[0] = 0.2f; ambient[1] = 0.2f; ambient[2] = 0.1f; ambient[3] = 1.0f;
                    diffuse[0] = 0.9f; diffuse[1] = 0.9f; diffuse[2] = 0.6f; diffuse[3] = 1.0f;
                    specular[0] = 0.4f; specular[1] = 0.4f; specular[2] = 0.2f; specular[3] = 1.0f;
                    break;
                case 2: // Luz brillante - blanca
                    ambient[0] = 0.3f; ambient[1] = 0.3f; ambient[2] = 0.3f; ambient[3] = 1.0f;
                    diffuse[0] = 1.0f; diffuse[1] = 1.0f; diffuse[2] = 0.9f; diffuse[3] = 1.0f;
                    specular[0] = 0.5f; specular[1] = 0.5f; specular[2] = 0.5f; specular[3] = 1.0f;
                    break;
            }
            
            // Aplicar intensidad
            for (int j = 0; j < 3; j++) {
                ambient[j] *= lightPoints[i].intensity;
                diffuse[j] *= lightPoints[i].intensity;
                specular[j] *= lightPoints[i].intensity;
            }
            
            glLightfv(GL_LIGHT0 + light_index, GL_AMBIENT, ambient);
            glLightfv(GL_LIGHT0 + light_index, GL_DIFFUSE, diffuse);
            glLightfv(GL_LIGHT0 + light_index, GL_SPECULAR, specular);
            
            // Configurar atenuación
            glLightf(GL_LIGHT0 + light_index, GL_CONSTANT_ATTENUATION, 1.0f);
            glLightf(GL_LIGHT0 + light_index, GL_LINEAR_ATTENUATION, 0.1f);
            glLightf(GL_LIGHT0 + light_index, GL_QUADRATIC_ATTENUATION, 0.01f);
            
            light_index++;
        }
    }
    
    // Deshabilitar luces no utilizadas
    for (int i = light_index; i < 8; i++) {
        glDisable(GL_LIGHT0 + i);
    }
}

void cleanup_renderer() {
    // Limpiar recursos de renderizado
    if (loading_texture != 0) {
        glDeleteTextures(1, &loading_texture);
        loading_texture = 0;
    }
}
