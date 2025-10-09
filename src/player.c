// player.c - Sistema de jugador 3D para Backrooms
#include "player.h"
#include "map.h"
#include "input.h"
#include "audio.h"
#include <stdio.h>
#include <math.h>

// Definir M_PI si no está definido
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Variables globales del jugador
Player3D player = {0};

// Usar la función is_wall del módulo de mapas

void init_player() {
    // Inicializar jugador con valores por defecto
    // Posición en el centro del mapa 200x200
    player.x = MAZE_WIDTH / 2.0f;
    player.y = 1.0f;  // Altura inicial más alta para evitar problemas de cámara
    player.z = MAZE_HEIGHT / 2.0f;
    player.yaw = 0.0f;
    player.pitch = 0.0f;
    player.height = 1.8f;  // Altura del jugador 1.80m
    player.rotSpeed = 0.02f;
    player.mouseSensitivity = 0.0005f;  // Sensibilidad reducida para movimiento más suave
    
    // Inicializar sistema de física
    player.velocityY = 0.0f;
    player.isGrounded = true;
    player.canJump = true;
    player.jumpForce = 0.18f;  // Fuerza de salto aumentada
    player.gravity = -0.006f;  // Gravedad reducida para salto más flotante
    
    // Inicializar sistema de movimiento (solo caminar)
    player.walkSpeed = 0.08f;  // Velocidad de caminata aumentada
    player.moveSpeed = player.walkSpeed;  // Usar la misma velocidad
    
    // Inicializar sistema de audio
    init_audio();
    
    // Reproducir sonido ambiental
    play_ambient_sound();
}

void update_player() {
    // Actualizar posición del jugador basado en input
    handle_movement();
    handle_jumping();
    apply_gravity();
    
    // Ajustar altura del jugador basada en el terreno
    adjust_player_to_terrain();
    
    // Actualizar sistema de audio
    update_audio();
}

void adjust_player_to_terrain() {
    // Obtener la altura del terreno en la posición actual del jugador
    extern float get_terrain_height(float x, float z);
    float terrain_height = get_terrain_height(player.x, player.z);
    
    // Ajustar la altura del jugador para que esté sobre el terreno
    // Mantener la altura mínima del jugador
    float min_height = terrain_height + player.height;
    
    // Si el jugador está por debajo del terreno, elevarlo
    if (player.y < min_height) {
        player.y = min_height;
        player.velocityY = 0.0f; // Detener velocidad vertical
        player.isGrounded = true;
    }
    
    // Si el jugador está muy por encima del terreno, aplicar gravedad suave
    else if (player.y > min_height + 0.5f) {
        player.velocityY -= 0.002f; // Gravedad suave hacia el terreno
    }
}

void handle_movement() {
    float newX = player.x;
    float newZ = player.z;
    bool moved = false;
    
    // Calcular dirección de movimiento basada en yaw (rotación horizontal)
    // Sistema de coordenadas: yaw=0 mira hacia Z negativo (adelante)
            // Usar la velocidad de caminata
            float currentSpeed = player.walkSpeed;
    
    // Adelante: hacia la dirección que mira la cámara
    float forwardX = -sin(player.yaw) * currentSpeed;
    float forwardZ = -cos(player.yaw) * currentSpeed;
    // Derecha: perpendicular a la dirección de la cámara
    float rightX = cos(player.yaw) * currentSpeed;
    float rightZ = -sin(player.yaw) * currentSpeed;
    
    // Verificar todas las teclas de movimiento presionadas
    if (is_key_pressed(GLFW_KEY_W)) {
        // W: Adelante (hacia donde mira la cámara)
        newX += forwardX;
        newZ += forwardZ;
        moved = true;
    }
    if (is_key_pressed(GLFW_KEY_S)) {
        // S: Atrás (opuesto a donde mira la cámara)
        newX -= forwardX;
        newZ -= forwardZ;
        moved = true;
    }
    if (is_key_pressed(GLFW_KEY_A)) {
        // A: Izquierda (perpendicular a la cámara)
        newX -= rightX;
        newZ -= rightZ;
        moved = true;
    }
    if (is_key_pressed(GLFW_KEY_D)) {
        // D: Derecha (perpendicular a la cámara)
        newX += rightX;
        newZ += rightZ;
        moved = true;
    }
    
            // Si se presionó alguna tecla de movimiento
            if (moved) {
                // Verificar colisiones con el nuevo sistema robusto
                if (!check_collision(newX, newZ)) {
                    // No hay colisión, permitir movimiento completo
                    player.x = newX;
                    player.z = newZ;
                    
                    // Reproducir sonido de pasos ocasionalmente
                    static int step_counter = 0;
                    step_counter++;
                    
                    // Sonido de pasos al caminar
                    if (step_counter % 30 == 0) { // Normal al caminar
                        play_footstep_sound();
                    }
                } else {
                    // Hay colisión, intentar movimiento solo en X o solo en Z
                    float newXOnly = player.x + (newX - player.x);
                    float newZOnly = player.z + (newZ - player.z);
                    
                    if (!check_collision(newXOnly, player.z)) {
                        // Movimiento solo en X es válido
                        player.x = newXOnly;
                    }
                    if (!check_collision(player.x, newZOnly)) {
                        // Movimiento solo en Z es válido
                        player.z = newZOnly;
                    }
                }
            }
}

void handle_rotation(float deltaX, float deltaY) {
    // Rotación horizontal (yaw) - estándar: mouse derecha = yaw negativo
    player.yaw -= deltaX * player.mouseSensitivity;
    
    // Rotación vertical (pitch) con límites - invertir eje Y para dirección natural
    player.pitch -= deltaY * player.mouseSensitivity;
    // Límites más estrictos para evitar problemas de renderizado
    if (player.pitch > M_PI/2 - 0.1f) player.pitch = M_PI/2 - 0.1f;  // Evitar mirar exactamente hacia arriba
    if (player.pitch < -M_PI/2 + 0.1f) player.pitch = -M_PI/2 + 0.1f;  // Evitar mirar exactamente hacia abajo
}

bool check_collision(float newX, float newZ) {
    // Radio del jugador más pequeño para movimiento más fluido
    float playerRadius = 0.2f;
    
    // Verificar solo el centro del jugador y puntos cardinales
    // Centro - convertir coordenadas del mundo a coordenadas del mapa
    int mapX = (int)(newX + 0.5f);
    int mapZ = (int)(newZ + 0.5f);
    
    if (is_wall(mapX, mapZ)) {
        return true;
    }
    
    // Puntos cardinales
    if (is_wall((int)(newX + playerRadius + 0.5f), (int)(newZ + 0.5f))) {
        return true;
    }
    if (is_wall((int)(newX - playerRadius + 0.5f), (int)(newZ + 0.5f))) {
        return true;
    }
    if (is_wall((int)(newX + 0.5f), (int)(newZ + playerRadius + 0.5f))) {
        return true;
    }
    if (is_wall((int)(newX + 0.5f), (int)(newZ - playerRadius + 0.5f))) {
        return true;
    }
    
    return false;
}


void handle_jumping() {
    // Verificar si se presiona la tecla de salto (ESPACIO)
    if (is_key_pressed(GLFW_KEY_SPACE) && player.isGrounded && player.canJump) {
        player.velocityY = player.jumpForce;
        player.isGrounded = false;
        player.canJump = false;
    }
}

void apply_gravity() {
    // Aplicar gravedad
    player.velocityY += player.gravity;
    
    // Calcular nueva posición Y
    float newY = player.y + player.velocityY;
    
    // Verificar colisión con el suelo
    if (newY <= 0.0f) {
        // El jugador toca el suelo
        player.y = 0.0f;
        player.velocityY = 0.0f;
        player.isGrounded = true;
        player.canJump = true;
    } else {
        // El jugador está en el aire
        player.y = newY;
        player.isGrounded = false;
    }
}

bool check_ground_collision(float newX, float newY, float newZ) {
    // Verificar si hay colisión con el suelo o paredes
    if (newY <= 0.0f) {
        return true; // Colisión con el suelo
    }
    
    // Verificar colisión con paredes (solo en X y Z)
    return check_collision(newX, newZ);
}

void cleanup_player() {
    // Limpiar recursos del jugador (si los hay)
}
