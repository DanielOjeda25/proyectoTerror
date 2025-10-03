// player.h - Sistema de jugador 3D para Backrooms
#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

// Estructura del jugador 3D
typedef struct {
    float x, y, z;              // Posición 3D
    float yaw, pitch;           // Rotación (horizontal y vertical)
    float height;               // Altura del jugador
    float moveSpeed;            // Velocidad de movimiento
    float rotSpeed;             // Velocidad de rotación
    float mouseSensitivity;     // Sensibilidad del mouse
    
    // Sistema de física
    float velocityY;            // Velocidad vertical
    bool isGrounded;            // Si está en el suelo
    bool canJump;               // Si puede saltar
    float jumpForce;            // Fuerza del salto
    float gravity;              // Fuerza de gravedad
    
    // Sistema de movimiento (solo caminar)
    float walkSpeed;            // Velocidad de caminata
} Player3D;

// Funciones del jugador
void init_player();
void update_player();
void handle_movement();
void handle_rotation(float deltaX, float deltaY);
void handle_jumping();
void apply_gravity();
bool check_collision(float newX, float newZ);
bool check_ground_collision(float newX, float newY, float newZ);
void cleanup_player();

// Variables globales del jugador
extern Player3D player;

#endif // PLAYER_H
