// main.c - Motor 3D para Laberintos Backrooms (Versión Modular)
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

// Incluir módulos del sistema
#include "player.h"
#include "input.h"
#include "render.h"
#include "map.h"
#include "events.h"

// Variables globales
GLFWwindow* window;
int windowWidth = 1920;
int windowHeight = 1080;

// Función para configurar OpenGL 3D
void setup_opengl() {
    // Configurar OpenGL para renderizado 3D
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)windowWidth / (float)windowHeight;
    gluPerspective(60.0f, aspect, 0.1f, 100.0f);  // FOV 60°, near 0.1, far 100
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Ajustar viewport
    glViewport(0, 0, windowWidth, windowHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Fondo negro
}

// Callback de redimensionado
void window_size_callback(GLFWwindow* window, int width, int height) {
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
    
    // Reconfigurar proyección
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)width / (float)height;
    gluPerspective(60.0f, aspect, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

int main() {
    // Inicializar GLFW
    if (!glfwInit()) {
        printf("Error al inicializar GLFW\n");
        return -1;
    }
    
    // Crear ventana
    window = glfwCreateWindow(windowWidth, windowHeight, "Deeper - Laberinto 3D", NULL, NULL);
    if (!window) {
        printf("No se pudo crear la ventana\n");
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback(window, window_size_callback);
    
    // Configurar OpenGL
    setup_opengl();
    
    // Inicializar sistemas modulares
    init_input();
    init_player();
    init_map();
    init_renderer();
    init_events();
    
    // Configurar callbacks de input
    setup_input_callbacks(window);
    
    printf("=== MOTOR 3D - BACKROOMS (MODULAR) ===\n");
    printf("Laberinto 3D generado dinámicamente con salidas garantizadas\n");
    printf("Controles:\n");
    printf("WASD - Mover (adelante/atrás/izquierda/derecha)\n");
    printf("Flechas - Rotar cámara\n");
    printf("Mouse - Rotar cámara 3D (click izquierdo para activar)\n");
    printf("ESPACIO - Saltar (próximamente)\n");
    printf("SHIFT - Agacharse (próximamente)\n");
    printf("ESC - Liberar mouse / Salir\n");
    printf("Posición inicial: (%.1f, %.1f, %.1f)\n", player.x, player.y, player.z);
    printf("Escala 3D: 25x25 - Altura 3 unidades\n");
    printf("Sistema modular inicializado\n");
    
    // Loop principal
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Actualizar sistemas
        update_player();
        process_events();
        
        // Renderizar mundo 3D
        render_world();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Limpiar recursos
    cleanup_player();
    cleanup_input();
    cleanup_map();
    cleanup_renderer();
    cleanup_events();
    
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}