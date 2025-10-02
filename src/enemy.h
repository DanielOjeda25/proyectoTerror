// enemy.h - Sistema de enemigo para Backrooms
#ifndef ENEMY_H
#define ENEMY_H

#include <stdbool.h>

// Estructura del enemigo
typedef struct {
    float x, z;           // Posición en el mapa
    float target_x, target_z; // Posición objetivo
    float speed;          // Velocidad de movimiento
    bool active;          // Si el enemigo está activo
    bool is_hunting;      // Si está cazando al jugador
    float detection_range; // Rango de detección
    float attack_range;   // Rango de ataque
    int behavior_timer;   // Temporizador para cambio de comportamiento
    int teleport_cooldown; // Cooldown para teletransporte
    bool was_hunting;     // Si estaba cazando en el frame anterior
    
    // Sistema de IA
    int ai_decision_timer; // Temporizador para decisiones de IA
    float suspicion_level; // Nivel de sospecha (0.0 - 1.0)
    bool is_stalking;     // Si está acechando al jugador
    int stalking_timer;   // Temporizador de acecho
    float last_player_x, last_player_z; // Última posición conocida del jugador
} Enemy;

// Variables globales del enemigo
extern Enemy enemy;

// Funciones del enemigo
void init_enemy();
void update_enemy();
void render_enemy_minimap();
void render_enemy_3d();
void check_enemy_collision();
bool is_player_dead();

#endif // ENEMY_H
