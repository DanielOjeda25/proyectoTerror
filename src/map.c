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
    
    // Generar patrón base más dinámico y aleatorio
    int patternType = rand() % 4; // 4 tipos diferentes de patrones
    
    switch (patternType) {
        case 0: // Patrón de laberinto clásico
            generate_classic_maze();
            break;
        case 1: // Patrón de habitaciones conectadas
            generate_room_maze();
            break;
        case 2: // Patrón de pasillos serpenteantes
            generate_winding_corridors();
            break;
        case 3: // Patrón de backrooms puro (más caótico)
            generate_backrooms_pattern();
            break;
    }
    
    // Asegurar que el jugador tenga espacio y conexión (centro del mapa 50x50)
    // Crear un área más grande libre alrededor del jugador
    int centerX = MAZE_WIDTH / 2;
    int centerY = MAZE_HEIGHT / 2;
    for (int y = centerY - 3; y <= centerY + 3; y++) {
        for (int x = centerX - 3; x <= centerX + 3; x++) {
            maze[y][x] = 0; // Área libre 7x7
        }
    }
    
    // Garantizar UNA SOLA salida accesible en un solo borde
    ensure_single_exit();
    
    // Añadir elementos decorativos aleatorios
    add_decorative_elements();
}

void generate_classic_maze() {
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
}

void generate_room_maze() {
    // Crear habitaciones de diferentes tamaños
    for (int room = 0; room < 8; room++) {
        int roomX = rand() % (MAZE_WIDTH - 6) + 2;
        int roomY = rand() % (MAZE_HEIGHT - 6) + 2;
        int roomW = rand() % 4 + 3; // 3-6 de ancho
        int roomH = rand() % 4 + 3; // 3-6 de alto
        
        // Asegurar que la habitación quepa
        if (roomX + roomW < MAZE_WIDTH - 1 && roomY + roomH < MAZE_HEIGHT - 1) {
            for (int y = roomY; y < roomY + roomH; y++) {
                for (int x = roomX; x < roomX + roomW; x++) {
                    maze[y][x] = 0;
                }
            }
        }
    }
    
    // Conectar habitaciones con pasillos
    for (int i = 0; i < 15; i++) {
        int x = rand() % (MAZE_WIDTH - 2) + 1;
        int y = rand() % (MAZE_HEIGHT - 2) + 1;
        maze[y][x] = 0;
    }
}

void generate_winding_corridors() {
    // Crear pasillos serpenteantes
    int startX = MAZE_WIDTH / 2;
    int startY = MAZE_HEIGHT / 2;
    int currentX = startX;
    int currentY = startY;
    
    for (int steps = 0; steps < 200; steps++) {
        maze[currentY][currentX] = 0;
        
        int direction = rand() % 4;
        switch (direction) {
            case 0: // Norte
                if (currentY > 1) currentY--;
                break;
            case 1: // Sur
                if (currentY < MAZE_HEIGHT - 2) currentY++;
                break;
            case 2: // Este
                if (currentX < MAZE_WIDTH - 2) currentX++;
                break;
            case 3: // Oeste
                if (currentX > 1) currentX--;
                break;
        }
    }
}

void generate_backrooms_pattern() {
    // Patrón más caótico estilo backrooms
    for (int y = 1; y < MAZE_HEIGHT - 1; y++) {
        for (int x = 1; x < MAZE_WIDTH - 1; x++) {
            if (rand() % 3 == 0) { // 33% de probabilidad de espacio libre
                maze[y][x] = 0;
            }
        }
    }
    
    // Crear columnas aleatorias variadas para soledad
    for (int y = 2; y < MAZE_HEIGHT - 2; y += 2) {
        for (int x = 2; x < MAZE_WIDTH - 2; x += 2) {
            if (rand() % 4 == 0) { // 25% de probabilidad de columna
                maze[y][x] = 1;
                
                // A veces crear columnas más gruesas (2x2)
                if (rand() % 3 == 0 && x < MAZE_WIDTH - 3 && y < MAZE_HEIGHT - 3) {
                    maze[y+1][x] = 1;
                    maze[y][x+1] = 1;
                    maze[y+1][x+1] = 1;
                }
            }
        }
    }
}

void ensure_single_exit() {
    // Seleccionar UN SOLO borde aleatoriamente para la salida
    int exit_side = rand() % 4; // 0=Norte, 1=Sur, 2=Este, 3=Oeste
    int exit_pos = 0;
    
    switch (exit_side) {
        case 0: // Norte
            exit_pos = rand() % (MAZE_WIDTH - 4) + 2; // Evitar esquinas
            maze[0][exit_pos] = 0;
            maze[1][exit_pos] = 0;
            maze[2][exit_pos] = 0; // Hacer la salida más ancha
            break;
        case 1: // Sur
            exit_pos = rand() % (MAZE_WIDTH - 4) + 2;
            maze[MAZE_HEIGHT - 1][exit_pos] = 0;
            maze[MAZE_HEIGHT - 2][exit_pos] = 0;
            maze[MAZE_HEIGHT - 3][exit_pos] = 0;
            break;
        case 2: // Este
            exit_pos = rand() % (MAZE_HEIGHT - 4) + 2;
            maze[exit_pos][MAZE_WIDTH - 1] = 0;
            maze[exit_pos][MAZE_WIDTH - 2] = 0;
            maze[exit_pos][MAZE_WIDTH - 3] = 0;
            break;
        case 3: // Oeste
            exit_pos = rand() % (MAZE_HEIGHT - 4) + 2;
            maze[exit_pos][0] = 0;
            maze[exit_pos][1] = 0;
            maze[exit_pos][2] = 0;
            break;
    }
    
    // Crear un laberinto complejo que conecte la salida con el centro
    create_complex_path_to_exit(exit_side, exit_pos);
}

void create_main_corridors(int exits[4]) {
    // Pasillo principal norte-sur
    for (int y = 1; y < MAZE_HEIGHT - 1; y++) {
        if (rand() % 3 == 0) { // 33% de probabilidad
            maze[y][exits[0]] = 0; // Conectar con salida norte
            maze[y][exits[1]] = 0; // Conectar con salida sur
        }
    }
    
    // Pasillo principal este-oeste
    for (int x = 1; x < MAZE_WIDTH - 1; x++) {
        if (rand() % 3 == 0) { // 33% de probabilidad
            maze[exits[2]][x] = 0; // Conectar con salida este
            maze[exits[3]][x] = 0; // Conectar con salida oeste
        }
    }
}

void connect_exits_to_center(int exits[4]) {
    // Conectar norte al centro
    for (int i = 0; i < 3; i++) {
        if (exits[0] > 1) maze[1][exits[0] - 1] = 0;
        if (exits[0] < MAZE_WIDTH - 2) maze[1][exits[0] + 1] = 0;
    }
    
    // Conectar sur al centro
    for (int i = 0; i < 3; i++) {
        if (exits[1] > 1) maze[MAZE_HEIGHT - 2][exits[1] - 1] = 0;
        if (exits[1] < MAZE_WIDTH - 2) maze[MAZE_HEIGHT - 2][exits[1] + 1] = 0;
    }
    
    // Conectar este al centro
    for (int i = 0; i < 3; i++) {
        if (exits[2] > 1) maze[exits[2] - 1][MAZE_WIDTH - 2] = 0;
        if (exits[2] < MAZE_HEIGHT - 2) maze[exits[2] + 1][MAZE_WIDTH - 2] = 0;
    }
    
    // Conectar oeste al centro
    for (int i = 0; i < 3; i++) {
        if (exits[3] > 1) maze[exits[3] - 1][1] = 0;
        if (exits[3] < MAZE_HEIGHT - 2) maze[exits[3] + 1][1] = 0;
    }
    
    // Crear pasillos principales hacia el centro desde cada salida
    create_main_corridors(exits);
}

void create_complex_path_to_exit(int exit_side, int exit_pos) {
    int centerX = MAZE_WIDTH / 2;
    int centerY = MAZE_HEIGHT / 2;
    
    // Crear un camino serpenteante y complejo desde el centro hacia la salida
    int currentX = centerX;
    int currentY = centerY;
    int targetX = 0, targetY = 0; // Inicializar las variables
    
    // Determinar la dirección hacia la salida
    switch (exit_side) {
        case 0: // Norte
            targetX = exit_pos;
            targetY = 0;
            break;
        case 1: // Sur
            targetX = exit_pos;
            targetY = MAZE_HEIGHT - 1;
            break;
        case 2: // Este
            targetX = MAZE_WIDTH - 1;
            targetY = exit_pos;
            break;
        case 3: // Oeste
            targetX = 0;
            targetY = exit_pos;
            break;
    }
    
    // Crear camino principal con desvíos y callejones sin salida
    for (int step = 0; step < 200; step++) {
        // Asegurar que el camino actual esté libre
        maze[currentY][currentX] = 0;
        
        // Calcular dirección hacia el objetivo
        int dirX = (targetX > currentX) ? 1 : (targetX < currentX) ? -1 : 0;
        int dirY = (targetY > currentY) ? 1 : (targetY < currentY) ? -1 : 0;
        
        // A veces tomar un desvío aleatorio para hacer el camino más complejo
        if (rand() % 4 == 0) {
            dirX = (rand() % 3) - 1; // -1, 0, o 1
            dirY = (rand() % 3) - 1;
        }
        
        // Mover hacia la dirección calculada
        int newX = currentX + dirX;
        int newY = currentY + dirY;
        
        // Verificar límites
        if (newX > 0 && newX < MAZE_WIDTH - 1 && newY > 0 && newY < MAZE_HEIGHT - 1) {
            currentX = newX;
            currentY = newY;
        }
        
        // Si llegamos cerca del objetivo, terminar
        if (abs(currentX - targetX) <= 2 && abs(currentY - targetY) <= 2) {
            break;
        }
    }
    
    // Crear callejones sin salida para aumentar la dificultad
    create_dead_ends();
}

void create_dead_ends() {
    // Crear callejones sin salida para hacer el laberinto más desafiante
    for (int i = 0; i < 30; i++) {
        int startX = rand() % (MAZE_WIDTH - 4) + 2;
        int startY = rand() % (MAZE_HEIGHT - 4) + 2;
        
        if (maze[startY][startX] == 0) { // Empezar desde un pasillo existente
            int length = rand() % 8 + 3; // Longitud del callejón 3-10
            int direction = rand() % 4;
            
            for (int j = 0; j < length; j++) {
                switch (direction) {
                    case 0: // Norte
                        if (startY - j > 0) maze[startY - j][startX] = 0;
                        break;
                    case 1: // Sur
                        if (startY + j < MAZE_HEIGHT - 1) maze[startY + j][startX] = 0;
                        break;
                    case 2: // Este
                        if (startX + j < MAZE_WIDTH - 1) maze[startY][startX + j] = 0;
                        break;
                    case 3: // Oeste
                        if (startX - j > 0) maze[startY][startX - j] = 0;
                        break;
                }
            }
        }
    }
}

void add_decorative_elements() {
    // Añadir elementos decorativos aleatorios para ambiente backrooms
    for (int i = 0; i < 25; i++) {
        int x = rand() % (MAZE_WIDTH - 2) + 1;
        int y = rand() % (MAZE_HEIGHT - 2) + 1;
        if (maze[y][x] == 0) { // Solo en espacios vacíos
            // Crear pequeñas estructuras decorativas
            if (rand() % 3 == 0) {
                maze[y][x] = 2; // Marcar como elemento decorativo
            }
        }
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
