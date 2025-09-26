// map.c - Sistema de mapas para Backrooms 3D
#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Variables globales del mapa
int maze[MAZE_HEIGHT][MAZE_WIDTH];

void init_map() {
    // Inicializar sistema de mapas
    srand((unsigned int)time(NULL));
    generate_map();
}

void generate_map() {
    // Inicializar todo como paredes
    for (int y = 0; y < MAZE_HEIGHT; y++) {
        for (int x = 0; x < MAZE_WIDTH; x++) {
            maze[y][x] = 1;
        }
    }
    
    // Crear pasillos principales (laberinto básico)
    for (int y = 1; y < MAZE_HEIGHT - 1; y += 2) {
        for (int x = 1; x < MAZE_WIDTH - 1; x++) {
            maze[y][x] = 0; // Pasillo horizontal
        }
    }
    
    // Crear pasillos verticales
    for (int x = 1; x < MAZE_WIDTH - 1; x += 2) {
        for (int y = 1; y < MAZE_HEIGHT - 1; y++) {
            if (rand() % 3 == 0) { // 33% de probabilidad de pasillo vertical
                maze[y][x] = 0;
            }
        }
    }
    
    // Crear columnas aleatorias variadas para soledad
    for (int y = 2; y < MAZE_HEIGHT - 2; y += 3) {
        for (int x = 2; x < MAZE_WIDTH - 2; x += 3) {
            if (rand() % 3 == 0) { // 33% de probabilidad de columna
                maze[y][x] = 1;
                
                // A veces crear columnas más gruesas (2x2)
                if (rand() % 4 == 0 && x < MAZE_WIDTH - 3 && y < MAZE_HEIGHT - 3) {
                    maze[y+1][x] = 1;
                    maze[y][x+1] = 1;
                    maze[y+1][x+1] = 1;
                }
            }
        }
    }
    
    // Asegurar que el jugador tenga espacio y conexión (centro del mapa 25x25)
    // Crear un área más grande libre alrededor del jugador
    for (int y = 10; y <= 14; y++) {
        for (int x = 10; x <= 14; x++) {
            maze[y][x] = 0; // Área libre 5x5
        }
    }
    
    // Crear al menos una salida en cada borde
    // Salida norte
    int northExit = rand() % (MAZE_WIDTH - 2) + 1;
    maze[0][northExit] = 0;
    maze[1][northExit] = 0;
    
    // Salida sur
    int southExit = rand() % (MAZE_WIDTH - 2) + 1;
    maze[MAZE_HEIGHT - 1][southExit] = 0;
    maze[MAZE_HEIGHT - 2][southExit] = 0;
    
    // Salida este
    int eastExit = rand() % (MAZE_HEIGHT - 2) + 1;
    maze[eastExit][MAZE_WIDTH - 1] = 0;
    maze[eastExit][MAZE_WIDTH - 2] = 0;
    
    // Salida oeste
    int westExit = rand() % (MAZE_HEIGHT - 2) + 1;
    maze[westExit][0] = 0;
    maze[westExit][1] = 0;
    
    // Conectar las salidas con pasillos
    for (int i = 0; i < 3; i++) {
        // Conectar norte
        if (northExit > 1) maze[1][northExit - 1] = 0;
        if (northExit < MAZE_WIDTH - 2) maze[1][northExit + 1] = 0;
        
        // Conectar sur
        if (southExit > 1) maze[MAZE_HEIGHT - 2][southExit - 1] = 0;
        if (southExit < MAZE_WIDTH - 2) maze[MAZE_HEIGHT - 2][southExit + 1] = 0;
        
        // Conectar este
        if (eastExit > 1) maze[eastExit - 1][MAZE_WIDTH - 2] = 0;
        if (eastExit < MAZE_HEIGHT - 2) maze[eastExit + 1][MAZE_WIDTH - 2] = 0;
        
        // Conectar oeste
        if (westExit > 1) maze[westExit - 1][1] = 0;
        if (westExit < MAZE_HEIGHT - 2) maze[westExit + 1][1] = 0;
    }
}

bool is_wall(int x, int z) {
    if (x < 0 || x >= MAZE_WIDTH || z < 0 || z >= MAZE_HEIGHT) {
        return true; // Fuera del mapa = pared
    }
    return maze[z][x] == 1;
}

void cleanup_map() {
    // Limpiar recursos del mapa (si los hay)
}
