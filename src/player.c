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
    player.x = 12.0f;
    player.y = 0.0f;
    player.z = 12.0f;
    player.yaw = 0.0f;
    player.pitch = 0.0f;
    player.height = 1.6f;
    player.moveSpeed = 0.001f;  // Velocidad original (más lenta)
    player.rotSpeed = 0.02f;
    player.mouseSensitivity = 0.001f;
}

void update_player() {
    // Actualizar posición del jugador basado en input
    handle_movement();
}

void handle_movement() {
    float newX = player.x;
    float newZ = player.z;
    bool moved = false;
    
    // Calcular dirección de movimiento basada en yaw (rotación horizontal)
    float forwardX = cos(player.yaw) * player.moveSpeed;
    float forwardZ = sin(player.yaw) * player.moveSpeed;
    float rightX = cos(player.yaw + M_PI/2) * player.moveSpeed;
    float rightZ = sin(player.yaw + M_PI/2) * player.moveSpeed;
    
    // Verificar todas las teclas de movimiento presionadas
    if (is_key_pressed(GLFW_KEY_W)) {
        newX += forwardX;
        newZ += forwardZ;
        moved = true;
    }
    if (is_key_pressed(GLFW_KEY_S)) {
        newX -= forwardX;
        newZ -= forwardZ;
        moved = true;
    }
    if (is_key_pressed(GLFW_KEY_A)) {
        newX -= rightX;  // A va a la izquierda
        newZ -= rightZ;
        moved = true;
    }
    if (is_key_pressed(GLFW_KEY_D)) {
        newX += rightX;  // D va a la derecha
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
    // Rotación horizontal (yaw)
    player.yaw += deltaX * player.mouseSensitivity;
    
    // Rotación vertical (pitch) con límites
    player.pitch -= deltaY * player.mouseSensitivity;
    if (player.pitch > M_PI/2) player.pitch = M_PI/2;
    if (player.pitch < -M_PI/2) player.pitch = -M_PI/2;
}

bool check_collision(float newX, float newZ) {
    // Radio del jugador ajustado al tamaño de la matriz (cada celda es 1x1)
    float playerRadius = 0.3f; // Radio que permite movimiento fluido en pasillos
    
    // Verificar el centro del jugador
    if (is_wall((int)(newX + 0.5f), (int)(newZ + 0.5f))) {
        return true;
    }
    
    // Verificar puntos alrededor del jugador para detectar paredes cercanas
    // Punto frontal (hacia donde se mueve)
    if (is_wall((int)(newX + playerRadius + 0.5f), (int)(newZ + 0.5f))) {
        return true;
    }
    // Punto trasero
    if (is_wall((int)(newX - playerRadius + 0.5f), (int)(newZ + 0.5f))) {
        return true;
    }
    // Punto izquierdo
    if (is_wall((int)(newX + 0.5f), (int)(newZ - playerRadius + 0.5f))) {
        return true;
    }
    // Punto derecho
    if (is_wall((int)(newX + 0.5f), (int)(newZ + playerRadius + 0.5f))) {
        return true;
    }
    
    // Verificar esquinas para evitar que se meta en las esquinas de las paredes
    // Esquina superior izquierda
    if (is_wall((int)(newX - playerRadius + 0.5f), (int)(newZ + playerRadius + 0.5f))) {
        return true;
    }
    // Esquina superior derecha
    if (is_wall((int)(newX + playerRadius + 0.5f), (int)(newZ + playerRadius + 0.5f))) {
        return true;
    }
    // Esquina inferior izquierda
    if (is_wall((int)(newX - playerRadius + 0.5f), (int)(newZ - playerRadius + 0.5f))) {
        return true;
    }
    // Esquina inferior derecha
    if (is_wall((int)(newX + playerRadius + 0.5f), (int)(newZ - playerRadius + 0.5f))) {
        return true;
    }
    
    return false;
}

void cleanup_player() {
    // Limpiar recursos del jugador (si los hay)
}
