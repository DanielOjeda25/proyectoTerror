// enemy.c - Sistema de enemigo para Backrooms
#include "enemy.h"
#include "player.h"
#include "map.h"
#include "render.h"
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
    
    enemy.x = (rand() % (MAZE_WIDTH - 20)) + 10;
    enemy.z = (rand() % (MAZE_HEIGHT - 20)) + 10;
    
    // Asegurar que no esté muy cerca del jugador
    while (sqrt((enemy.x - player.x) * (enemy.x - player.x) + 
                (enemy.z - player.z) * (enemy.z - player.z)) < 30.0f) {
        enemy.x = (rand() % (MAZE_WIDTH - 20)) + 10;
        enemy.z = (rand() % (MAZE_HEIGHT - 20)) + 10;
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
    
    printf("Enemigo inicializado en posición (%.1f, %.1f)\n", enemy.x, enemy.z);
}

void update_enemy() {
    if (!enemy.active) return;
    
    enemy.behavior_timer++;
    enemy.ai_decision_timer++;
    
    // Reducir cooldowns
    if (enemy.teleport_cooldown > 0) {
        enemy.teleport_cooldown--;
    }
    if (enemy.stalking_timer > 0) {
        enemy.stalking_timer--;
    }
    
    // Calcular distancia al jugador
    float distance_to_player = sqrt((enemy.x - player.x) * (enemy.x - player.x) + 
                                   (enemy.z - player.z) * (enemy.z - player.z));
    
    // Calcular movimiento del jugador para detectar actividad
    float player_movement = sqrt((player.x - enemy.last_player_x) * (player.x - enemy.last_player_x) + 
                                (player.z - enemy.last_player_z) * (player.z - enemy.last_player_z));
    
    // Actualizar última posición conocida del jugador
    enemy.last_player_x = player.x;
    enemy.last_player_z = player.z;
    
    // Sistema de IA: tomar decisiones cada 2 segundos (120 frames)
    if (enemy.ai_decision_timer >= 120) {
        enemy.ai_decision_timer = 0;
        
        // Calcular nivel de sospecha basado en distancia y movimiento
        if (distance_to_player <= enemy.detection_range) {
            // Aumentar sospecha si está cerca
            enemy.suspicion_level += 0.3f;
            
            // Aumentar sospecha si el jugador se está moviendo
            if (player_movement > 0.1f) {
                enemy.suspicion_level += 0.2f;
            }
        } else {
            // Reducir sospecha si está lejos
            enemy.suspicion_level -= 0.1f;
        }
        
        // Limitar nivel de sospecha entre 0 y 1
        if (enemy.suspicion_level < 0.0f) enemy.suspicion_level = 0.0f;
        if (enemy.suspicion_level > 1.0f) enemy.suspicion_level = 1.0f;
        
        // Decisión de IA basada en nivel de sospecha
        float decision = (float)rand() / RAND_MAX; // Número aleatorio entre 0 y 1
        
        if (enemy.suspicion_level > 0.7f && decision < 0.8f) {
            // Alta sospecha: 80% de probabilidad de cazar
            enemy.is_hunting = true;
            enemy.is_stalking = false;
            printf("ENEMIGO: ¡ALTA SOSPECHA! Nivel: %.2f - MODO CAZA\n", enemy.suspicion_level);
        } else if (enemy.suspicion_level > 0.4f && decision < 0.6f) {
            // Sospecha media: 60% de probabilidad de acechar
            enemy.is_hunting = false;
            enemy.is_stalking = true;
            enemy.stalking_timer = 300; // 5 segundos de acecho
            printf("ENEMIGO: Sospecha media (%.2f) - MODO ACECHO\n", enemy.suspicion_level);
        } else if (enemy.suspicion_level > 0.2f && decision < 0.3f) {
            // Baja sospecha: 30% de probabilidad de investigar
            enemy.is_hunting = false;
            enemy.is_stalking = true;
            enemy.stalking_timer = 180; // 3 segundos de acecho
            printf("ENEMIGO: Baja sospecha (%.2f) - INVESTIGANDO\n", enemy.suspicion_level);
        } else {
            // Sin sospecha: patrullar normalmente
            enemy.is_hunting = false;
            enemy.is_stalking = false;
        }
    }
    
    // Comportamiento basado en estado de IA
    if (enemy.is_hunting) {
        // Modo caza: teletransportarse cerca del jugador
        if (enemy.teleport_cooldown <= 0) {
            float angle = (rand() % 360) * M_PI / 180.0f;
            float distance = (rand() % 8) + 3; // Entre 3 y 11 unidades (más cerca)
            
            enemy.x = player.x + cos(angle) * distance;
            enemy.z = player.z + sin(angle) * distance;
            
            // Asegurar que esté dentro del mapa
            if (enemy.x < 5) enemy.x = 5;
            if (enemy.x > MAZE_WIDTH - 5) enemy.x = MAZE_WIDTH - 5;
            if (enemy.z < 5) enemy.z = 5;
            if (enemy.z > MAZE_HEIGHT - 5) enemy.z = MAZE_HEIGHT - 5;
            
            enemy.teleport_cooldown = 240; // 4 segundos de cooldown
            printf("¡ENEMIGO CAZANDO! Posición: (%.1f, %.1f)\n", enemy.x, enemy.z);
        }
    } else if (enemy.is_stalking) {
        // Modo acecho: teletransportarse a distancia media
        if (enemy.teleport_cooldown <= 0) {
            float angle = (rand() % 360) * M_PI / 180.0f;
            float distance = (rand() % 15) + 10; // Entre 10 y 25 unidades
            
            enemy.x = player.x + cos(angle) * distance;
            enemy.z = player.z + sin(angle) * distance;
            
            // Asegurar que esté dentro del mapa
            if (enemy.x < 5) enemy.x = 5;
            if (enemy.x > MAZE_WIDTH - 5) enemy.x = MAZE_WIDTH - 5;
            if (enemy.z < 5) enemy.z = 5;
            if (enemy.z > MAZE_HEIGHT - 5) enemy.z = MAZE_HEIGHT - 5;
            
            enemy.teleport_cooldown = 180; // 3 segundos de cooldown
            printf("Enemigo acechando en (%.1f, %.1f)\n", enemy.x, enemy.z);
        }
        
        // Terminar acecho después del tiempo
        if (enemy.stalking_timer <= 0) {
            enemy.is_stalking = false;
        }
    } else {
        // Modo patrulla: teletransportarse aleatoriamente cada 3-10 segundos
        if (enemy.behavior_timer % (rand() % 420 + 180) == 0 && enemy.teleport_cooldown <= 0) {
            // Teletransportarse a posición aleatoria
            enemy.x = (rand() % (MAZE_WIDTH - 20)) + 10;
            enemy.z = (rand() % (MAZE_HEIGHT - 20)) + 10;
            
            // Asegurar que no esté en un muro
            while (maze[(int)enemy.x][(int)enemy.z] == 1) {
                enemy.x = (rand() % (MAZE_WIDTH - 20)) + 10;
                enemy.z = (rand() % (MAZE_HEIGHT - 20)) + 10;
            }
            
            enemy.teleport_cooldown = 120; // 2 segundos de cooldown
            printf("Enemigo patrullando en (%.1f, %.1f)\n", enemy.x, enemy.z);
        }
    }
    
    // Verificar si está en rango de ataque
    if (distance_to_player <= enemy.attack_range) {
        printf("¡EL ENEMIGO TE HA ALCANZADO! ¡GAME OVER!\n");
        enemy.active = false; // Desactivar enemigo
        // El juego se cerrará en el bucle principal
    }
}

void render_enemy_minimap() {
    if (!enemy.active) return;
    
    // Obtener dimensiones de la ventana desde render.h
    extern int windowWidth, windowHeight;
    
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
        // Configurar material rojo brillante y pulsante
        float pulse = (sin(enemy.behavior_timer * 0.05f) + 1.0f) * 0.5f; // Pulsación más lenta
        float intensity = 0.8f + pulse * 0.2f; // Entre 0.8 y 1.0 (más brillante)
        
        GLfloat ambient[] = {0.6f * intensity, 0.0f, 0.0f, 1.0f};
        GLfloat diffuse[] = {1.0f * intensity, 0.0f, 0.0f, 1.0f};
        GLfloat specular[] = {1.0f * intensity, 0.2f, 0.2f, 1.0f};
        GLfloat shininess[] = {128.0f};
        
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
        
        // Dibujar cubo rojo flotante del enemigo
        glPushMatrix();
        float float_height = 2.0f + sin(enemy.behavior_timer * 0.05f) * 0.3f; // Flotación
        glTranslatef(enemy.x, float_height, enemy.z);
        
        // Rotación lenta para efecto hipnótico
        glRotatef(enemy.behavior_timer * 0.5f, 0, 1, 0);
        
        // Dibujar cubo rojo más grande y visible
        float size = 1.2f + pulse * 0.3f; // Tamaño pulsante
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
