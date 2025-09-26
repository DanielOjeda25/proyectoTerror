// input.c - Sistema de input para Backrooms 3D
#include "input.h"
#include "player.h"
#include <stdio.h>

// Definir M_PI si no está definido
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Variables globales de input
bool keys[GLFW_KEY_LAST + 1] = {false};
bool mouseCaptured = false;
double lastMouseX = 0.0, lastMouseY = 0.0;

void init_input() {
    // Inicializar sistema de input
    for (int i = 0; i < GLFW_KEY_LAST + 1; i++) {
        keys[i] = false;
    }
    mouseCaptured = false;
    lastMouseX = 0.0;
    lastMouseY = 0.0;
}

void setup_input_callbacks(GLFWwindow* window) {
    // Configurar callbacks de input
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
}

void process_input(GLFWwindow* window) {
    // Procesar entrada del usuario (se hace en callbacks)
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (!mouseCaptured) {
            mouseCaptured = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
        }
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (mouseCaptured) {
        double deltaX = xpos - lastMouseX;
        double deltaY = ypos - lastMouseY;
        
        // Usar el sistema de rotación del jugador
        handle_rotation(deltaX, deltaY);
        
        lastMouseX = xpos;
        lastMouseY = ypos;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        // Actualizar estado de teclas presionadas
        if (key >= 0 && key < GLFW_KEY_LAST) {
            keys[key] = true;
        }
        
        // Manejar teclas especiales
        switch (key) {
            case GLFW_KEY_LEFT:
                player.yaw -= player.rotSpeed;
                break;
            case GLFW_KEY_RIGHT:
                player.yaw += player.rotSpeed;
                break;
            case GLFW_KEY_UP:
                player.pitch += player.rotSpeed;
                if (player.pitch > M_PI/2) player.pitch = M_PI/2;
                break;
            case GLFW_KEY_DOWN:
                player.pitch -= player.rotSpeed;
                if (player.pitch < -M_PI/2) player.pitch = -M_PI/2;
                break;
            case GLFW_KEY_ESCAPE:
                if (mouseCaptured) {
                    mouseCaptured = false;
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                } else {
                    glfwSetWindowShouldClose(window, GLFW_TRUE);
                }
                break;
        }
    } else if (action == GLFW_RELEASE) {
        // Actualizar estado de teclas liberadas
        if (key >= 0 && key < GLFW_KEY_LAST) {
            keys[key] = false;
        }
    }
}

bool is_key_pressed(int key) {
    if (key >= 0 && key < GLFW_KEY_LAST) {
        return keys[key];
    }
    return false;
}

void cleanup_input() {
    // Limpiar recursos de input (si los hay)
}
