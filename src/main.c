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
#include "enemy.h"

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
    gluPerspective(90.0f, aspect, 0.1f, 1000.0f);  // FOV 90°, near 0.1, far 1000
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Ajustar viewport
    glViewport(0, 0, windowWidth, windowHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Fondo negro
    
    // Configurar depth testing correctamente
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    
    // Configurar culling para mejor rendimiento
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
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
    gluPerspective(90.0f, aspect, 0.1f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
}

int main() {
    // Inicializar GLFW
    if (!glfwInit()) {
        printf("Error al inicializar GLFW\n");
        return -1;
    }
    
    // Crear ventana en pantalla completa
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    
    window = glfwCreateWindow(mode->width, mode->height, "Deeper - Laberinto 3D", monitor, NULL);
    if (!window) {
        printf("No se pudo crear la ventana\n");
        glfwTerminate();
        return -1;
    }
    
    // Actualizar dimensiones para pantalla completa
    windowWidth = mode->width;
    windowHeight = mode->height;
    
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
    init_enemy();
    
    // Configurar callbacks de input
    setup_input_callbacks(window);
    
    printf("=== MOTOR 3D - BACKROOMS (MODULAR) ===\n");
    printf("Laberinto 3D generado dinámicamente con UNA SOLA salida\n");
    printf("Sistema de iluminación dinámica y niebla realista activado\n");
    printf("Controles:\n");
    printf("WASD - Mover (adelante/atrás/izquierda/derecha)\n");
    printf("Flechas - Rotar cámara\n");
    printf("Mouse - Rotar cámara 3D (click izquierdo para activar)\n");
    printf("ESPACIO - Saltar\n");
    printf("SHIFT - Agacharse (próximamente)\n");
    printf("ESC - Liberar mouse / Salir\n");
    printf("Posición inicial: (%.1f, %.1f, %.1f)\n", player.x, player.y, player.z);
    printf("Escala 3D: 200x200x8 - Altura %d unidades (MAPA MASIVO CON ALTURA)\n", MAZE_LEVELS);
    printf("Luz: Rango %.1f unidades - Niebla: %.1f-%.1f unidades\n", LIGHT_RANGE, FOG_START_DISTANCE, FOG_END_DISTANCE);
    printf("Carga progresiva: Solo se renderiza lo que está iluminado\n");
    printf("Sistema modular inicializado\n");
    
    // Loop principal
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Actualizar sistemas
        update_player();
        update_enemy();
        process_events();
        
        // Verificar si el jugador está muerto
        if (is_player_dead()) {
            printf("¡GAME OVER! El enemigo te ha alcanzado.\n");
            break;
        }
        
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