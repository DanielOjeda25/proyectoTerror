// enemy.c - Sistema de enemigo para Backrooms
#include "enemy.h"
#include "player.h"
#include "map.h"
#include "render.h"
#include "audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Variable global del enemigo
Enemy enemy;

void init_enemy() {
    // Inicializar enemigo en una posición aleatoria lejos del jugador
    srand((unsigned int)time(NULL));
    
    int attempts = 0;
    do {
        enemy.x = (rand() % (MAZE_WIDTH - 20)) + 10;
        enemy.z = (rand() % (MAZE_HEIGHT - 20)) + 10;
        attempts++;
    } while ((sqrt((enemy.x - player.x) * (enemy.x - player.x) + 
                   (enemy.z - player.z) * (enemy.z - player.z)) < 30.0f ||
             is_wall((int)enemy.x, (int)enemy.z)) && attempts < 100);
    
    // Si no se encontró una posición válida después de 100 intentos, usar posición por defecto
    if (attempts >= 100) {
        enemy.x = MAZE_WIDTH / 2.0f;
        enemy.z = MAZE_HEIGHT / 2.0f;
        printf("ADVERTENCIA: Enemigo colocado en posición por defecto\n");
    }
    
    enemy.target_x = enemy.x;
    enemy.target_z = enemy.z;
    enemy.speed = 0.05f; // Velocidad aumentada para mapa gigante
    enemy.active = true;
    enemy.is_hunting = false;
    enemy.detection_range = 25.0f; // Rango de detección aumentado
    enemy.attack_range = 3.0f;     // Rango de ataque aumentado
    enemy.behavior_timer = 0;
    enemy.teleport_cooldown = 0;
    enemy.was_hunting = false;
    
    // Inicializar sistema de IA
    enemy.ai_decision_timer = 0;
    enemy.suspicion_level = 0.0f;
    enemy.is_stalking = false;
    enemy.stalking_timer = 0;
    enemy.last_player_x = player.x;
    enemy.last_player_z = player.z;
    
    // Nuevo sistema de comportamiento escalonado
    enemy.phase = 0; // 0 = Teletransporte aleatorio, 1 = Acercamiento gradual
    enemy.phase_timer = 0;
    enemy.phase_duration = 3600; // 1 minuto = 3600 frames (60 FPS)
    enemy.teleport_frequency = 120; // Teletransportarse cada 2 segundos en fase 0
    enemy.last_teleport = 0;
    
    printf("Enemigo inicializado en posición (%.1f, %.1f) - FASE: Teletransporte Aleatorio\n", enemy.x, enemy.z);
}

void update_enemy() {
    if (!enemy.active) return;
    
    // Actualizar temporizadores
    enemy.behavior_timer++;
    enemy.phase_timer++;
    
    // Calcular distancia al jugador
    float distance_to_player = sqrt((enemy.x - player.x) * (enemy.x - player.x) + 
                                   (enemy.z - player.z) * (enemy.z - player.z));
    
    // Verificar si está en rango de ataque (siempre verificar)
    if (distance_to_player <= enemy.attack_range) {
        printf("¡EL ENEMIGO TE HA ALCANZADO! ¡GAME OVER!\n");
        play_death_sound();
        enemy.active = false;
        return;
    }
    
    // SISTEMA DE COMPORTAMIENTO ESCALONADO
    if (enemy.phase == 0) {
        // FASE 0: Teletransporte aleatorio por 1 minuto
        if (enemy.phase_timer >= enemy.phase_duration) {
            // Cambiar a fase 1 (acercamiento gradual)
            enemy.phase = 1;
            enemy.phase_timer = 0;
            printf("ENEMIGO: ¡CAMBIO DE FASE! Ahora comenzará a acercarse...\n");
        } else {
            // Teletransportarse aleatoriamente cada 2 segundos
            if (enemy.behavior_timer - enemy.last_teleport >= enemy.teleport_frequency) {
                int attempts = 0;
                do {
                    // Teletransportarse a posición aleatoria en el mapa
                    enemy.x = (rand() % (MAZE_WIDTH - 20)) + 10;
                    enemy.z = (rand() % (MAZE_HEIGHT - 20)) + 10;
                    attempts++;
                } while (is_wall((int)enemy.x, (int)enemy.z) && attempts < 50);
                
                enemy.last_teleport = enemy.behavior_timer;
                
                // Mostrar progreso cada 10 segundos
                int remaining_time = (enemy.phase_duration - enemy.phase_timer) / 60;
                if (enemy.phase_timer % 600 == 0) {
                    printf("ENEMIGO: Teletransporte aleatorio - Tiempo restante: %d segundos\n", remaining_time);
                }
            }
        }
    } else if (enemy.phase == 1) {
        // FASE 1: Acercamiento gradual
        if (enemy.behavior_timer % 180 == 0) { // Cada 3 segundos
            // Calcular dirección hacia el jugador
            float dx = player.x - enemy.x;
            float dz = player.z - enemy.z;
            float distance = sqrt(dx * dx + dz * dz);
            
            if (distance > 0.0f) {
                // Normalizar dirección
                dx /= distance;
                dz /= distance;
                
                // Calcular distancia de acercamiento basada en el tiempo en fase 1
                float phase_progress = (float)enemy.phase_timer / 1800.0f; // 30 segundos para acercamiento completo
                if (phase_progress > 1.0f) phase_progress = 1.0f;
                
                // Distancia de acercamiento: de 50 unidades a 5 unidades
                float min_distance = 50.0f - (45.0f * phase_progress);
                float max_distance = min_distance + 10.0f;
                
                // Calcular nueva posición
                float approach_distance = min_distance + (rand() % (int)(max_distance - min_distance));
                float new_x = player.x - dx * approach_distance;
                float new_z = player.z - dz * approach_distance;
                
                // Asegurar que esté dentro del mapa
                if (new_x < 5) new_x = 5;
                if (new_x > MAZE_WIDTH - 5) new_x = MAZE_WIDTH - 5;
                if (new_z < 5) new_z = 5;
                if (new_z > MAZE_HEIGHT - 5) new_z = MAZE_HEIGHT - 5;
                
                // Verificar que no esté en una pared
                if (!is_wall((int)new_x, (int)new_z)) {
                    enemy.x = new_x;
                    enemy.z = new_z;
                    
                    printf("ENEMIGO: Acercándose - Distancia: %.1f unidades\n", approach_distance);
                    
                    // Reproducir sonido de enemigo cuando se acerca
                    if (enemy.behavior_timer % 360 == 0) { // Cada 6 segundos
                        play_enemy_sound();
                    }
                }
            }
        }
    }
}

void render_enemy_minimap() {
    if (!enemy.active) return;
    
    // Obtener dimensiones de la ventana desde render.h
    extern int windowWidth;
    
    float minimapSize = 200.0f;
    float x = windowWidth - minimapSize - 10;
    float y = 10;
    float scale = minimapSize / MAZE_WIDTH;
    
    // Posición del enemigo en el mini mapa
    float enemyMapX = x + enemy.x * scale;
    float enemyMapY = y + enemy.z * scale;
    
    // Dibujar enemigo como un punto rojo más grande
    glDisable(GL_BLEND);
    glColor3f(1.0f, 0.0f, 0.0f); // Rojo brillante para el enemigo
    
    // Dibujar círculo del enemigo
    glPointSize(6.0f);
    glBegin(GL_POINTS);
    glVertex2f(enemyMapX, enemyMapY);
    glEnd();
    
    // Dibujar aura de peligro si está cazando
    if (enemy.is_hunting) {
        glColor3f(1.0f, 0.5f, 0.0f); // Naranja para aura de peligro
        glPointSize(10.0f);
        glBegin(GL_POINTS);
        glVertex2f(enemyMapX, enemyMapY);
        glEnd();
    }
}

void check_enemy_collision() {
    if (!enemy.active) return;
    
    float distance_to_player = sqrt((enemy.x - player.x) * (enemy.x - player.x) + 
                                   (enemy.z - player.z) * (enemy.z - player.z));
    
    if (distance_to_player <= enemy.attack_range) {
        printf("¡EL ENEMIGO TE HA ALCANZADO! ¡GAME OVER!\n");
        enemy.active = false;
    }
}

void render_enemy_3d() {
    if (!enemy.active) return;
    
    // Calcular distancia desde el jugador
    float distance = sqrt((enemy.x - player.x) * (enemy.x - player.x) + 
                         (enemy.z - player.z) * (enemy.z - player.z));
    
    // Renderizar si está a menos de 100 unidades (más visible)
    if (distance <= 100.0f) {
        // Configurar iluminación del enemigo
        float float_height = 2.0f + sin(enemy.behavior_timer * 0.05f) * 0.3f;
        setup_enemy_lighting(enemy.x, float_height, enemy.z);
        
        // Configurar material rojo brillante y pulsante
        float pulse = (sin(enemy.behavior_timer * 0.05f) + 1.0f) * 0.5f; // Pulsación más lenta
        float intensity = 1.0f + pulse * 0.5f; // Entre 1.0 y 1.5 (más brillante)
        
        GLfloat ambient[] = {0.8f * intensity, 0.0f, 0.0f, 1.0f};
        GLfloat diffuse[] = {1.2f * intensity, 0.0f, 0.0f, 1.0f};
        GLfloat specular[] = {1.0f * intensity, 0.3f, 0.3f, 1.0f};
        GLfloat shininess[] = {128.0f};
        
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
        
        // Dibujar cubo rojo flotante del enemigo
        glPushMatrix();
        
        // Animación de acercamiento cuando está cerca
        float approach_animation = 0.0f;
        if (distance <= 15.0f) {
            // Efecto de "respiración" más intenso cuando está cerca
            approach_animation = sin(enemy.behavior_timer * 0.2f) * 0.5f;
            float_height += approach_animation;
        }
        
        glTranslatef(enemy.x, float_height, enemy.z);
        
        // Rotación más rápida cuando está cerca
        float rotation_speed = 0.5f;
        if (distance <= 15.0f) {
            rotation_speed = 2.0f; // Rotación más rápida cuando está cerca
        }
        glRotatef(enemy.behavior_timer * rotation_speed, 0, 1, 0);
        
                // Dibujar cubo rojo más pequeño
                float size = 0.6f + pulse * 0.2f; // Tamaño pulsante más pequeño
                
                // Hacer el cubo un poco más grande cuando está cerca
                if (distance <= 15.0f) {
                    size += 0.2f + approach_animation * 0.1f; // Más pequeño cuando está cerca
                }
        glBegin(GL_QUADS);
        
        // Cara frontal (Z+)
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-size, -size, size);
        glVertex3f(size, -size, size);
        glVertex3f(size, size, size);
        glVertex3f(-size, size, size);
        
        // Cara trasera (Z-)
        glNormal3f(0.0f, 0.0f, -1.0f);
        glVertex3f(-size, -size, -size);
        glVertex3f(-size, size, -size);
        glVertex3f(size, size, -size);
        glVertex3f(size, -size, -size);
        
        // Cara izquierda (X-)
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(-size, -size, -size);
        glVertex3f(-size, -size, size);
        glVertex3f(-size, size, size);
        glVertex3f(-size, size, -size);
        
        // Cara derecha (X+)
        glNormal3f(1.0f, 0.0f, 0.0f);
        glVertex3f(size, -size, -size);
        glVertex3f(size, size, -size);
        glVertex3f(size, size, size);
        glVertex3f(size, -size, size);
        
        // Cara superior (Y+)
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-size, size, -size);
        glVertex3f(-size, size, size);
        glVertex3f(size, size, size);
        glVertex3f(size, size, -size);
        
        // Cara inferior (Y-)
        glNormal3f(0.0f, -1.0f, 0.0f);
        glVertex3f(-size, -size, -size);
        glVertex3f(size, -size, -size);
        glVertex3f(size, -size, size);
        glVertex3f(-size, -size, size);
        
        glEnd();
        glPopMatrix();
        
        // Dibujar aura roja pulsante
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        float aura_alpha = 0.2f + pulse * 0.3f; // Alpha pulsante
        glColor4f(1.0f, 0.0f, 0.0f, aura_alpha);
        
        glPushMatrix();
        glTranslatef(enemy.x, 0.1f, enemy.z);
        
        // Dibujar aura circular pulsante
        float aura_radius = 3.0f + pulse * 1.0f;
        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, 0, 0);
        for (int i = 0; i <= 32; i++) {
            float angle = 2.0f * M_PI * i / 32.0f;
            glVertex3f(cos(angle) * aura_radius, 0, sin(angle) * aura_radius);
        }
        glEnd();
        
        glPopMatrix();
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
    }
}

bool is_player_dead() {
    return !enemy.active; // El enemigo se desactiva cuando mata al jugador
}
