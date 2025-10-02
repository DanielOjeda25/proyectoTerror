// player.c - Sistema de jugador 3D para Backrooms
#include "player.h"
#include "map.h"
#include "input.h"
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
    player.y = 0.1f;  // Ligeramente por encima del suelo para evitar caída
    player.z = MAZE_HEIGHT / 2.0f;
    player.yaw = 0.0f;
    player.pitch = 0.0f;
    player.height = 1.6f;  // Altura del jugador en el nivel más bajo
    player.moveSpeed = 0.02f;  // Velocidad ajustada para movimiento perceptible
    player.rotSpeed = 0.02f;
    player.mouseSensitivity = 0.001f;  // Sensibilidad más suave y natural
    
    // Inicializar sistema de física
    player.velocityY = 0.0f;
    player.isGrounded = true;
    player.canJump = true;
    player.jumpForce = 0.15f;
    player.gravity = -0.008f;
}

void update_player() {
    // Actualizar posición del jugador basado en input
    handle_movement();
    handle_jumping();
    apply_gravity();
}

void handle_movement() {
    float newX = player.x;
    float newZ = player.z;
    bool moved = false;
    
    // Calcular dirección de movimiento basada en yaw (rotación horizontal)
    // Sistema de coordenadas estándar: yaw=0 mira hacia X positivo (adelante)
    // Adelante: hacia X positivo
    float forwardX = cos(player.yaw) * player.moveSpeed;
    float forwardZ = sin(player.yaw) * player.moveSpeed;
    // Derecha: perpendicular a la dirección de la cámara (hacia Z positivo)
    float rightX = -sin(player.yaw) * player.moveSpeed;
    float rightZ = cos(player.yaw) * player.moveSpeed;
    
    // Verificar todas las teclas de movimiento presionadas
    if (is_key_pressed(GLFW_KEY_W)) {
        // W: Adelante (hacia X positivo - estándar)
        newX += forwardX;
        newZ += forwardZ;
        moved = true;
    }
    if (is_key_pressed(GLFW_KEY_S)) {
        // S: Atrás (hacia X negativo - estándar)
        newX -= forwardX;
        newZ -= forwardZ;
        moved = true;
    }
    if (is_key_pressed(GLFW_KEY_A)) {
        // A: Izquierda (strafe left - hacia Z negativo - estándar)
        newX -= rightX;
        newZ -= rightZ;
        moved = true;
    }
    if (is_key_pressed(GLFW_KEY_D)) {
        // D: Derecha (strafe right - hacia Z positivo - estándar)
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
    // Rotación horizontal (yaw) - estándar: mouse derecha = yaw positivo
    player.yaw += deltaX * player.mouseSensitivity;
    
    // Rotación vertical (pitch) con límites - estándar: mouse arriba = pitch positivo
    player.pitch += deltaY * player.mouseSensitivity;
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
