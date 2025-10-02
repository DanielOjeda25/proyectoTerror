// map.c - Sistema de mapas para Backrooms 3D
#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Variables globales del mapa
int maze[MAZE_WIDTH][MAZE_HEIGHT];

// Estructuras de datos avanzadas
Room rooms[100];
Corridor corridors[200];
Column columns[150];
LightPoint lightPoints[50];
int roomCount = 0;
int corridorCount = 0;
int columnCount = 0;
int lightCount = 0;

// Variables globales para tracking de salida
static int exit_side = -1;
static int exit_pos = -1;

void init_map() {
    // Inicializar sistema de mapas
    srand((unsigned int)time(NULL));
    generate_map();
}

void generate_map() {
    // Inicializar todo como paredes
        for (int x = 0; x < MAZE_WIDTH; x++) {
        for (int z = 0; z < MAZE_HEIGHT; z++) {
            maze[x][z] = 1;
        }
    }
    
    // Resetear contadores
    roomCount = 0;
    corridorCount = 0;
    columnCount = 0;
    lightCount = 0;
    
    // PRIMERO: Crear área del jugador en el centro
    int centerX = MAZE_WIDTH / 2;
    int centerZ = MAZE_HEIGHT / 2;
        for (int x = centerX - 5; x <= centerX + 5; x++) {
        for (int z = centerZ - 5; z <= centerZ + 5; z++) {
            maze[x][z] = 0; // Área libre 11x11 (más ancha)
        }
    }
    
    // SEGUNDO: Crear UNA salida garantizada en un borde
    ensure_single_exit();
    
    // TERCERO: Usar el nuevo sistema de generación avanzada
    generate_advanced_backrooms();
    
    // CUARTO: Verificar conectividad y corregir si es necesario
    ensure_connectivity();
}

void generate_classic_maze() {
    // Crear pasillos principales de diferentes anchos (estilo backrooms)
    
    // Pasillos horizontales principales (muy anchos)
    for (int x = 5; x < MAZE_WIDTH - 5; x += 8) {
        for (int z = 5; z < MAZE_HEIGHT - 5; z++) {
            // Pasillo principal de 3-4 unidades de ancho
            maze[x][z] = 0;
            if (z < MAZE_HEIGHT - 6) maze[x][z+1] = 0;
            if (z < MAZE_HEIGHT - 7) maze[x][z+2] = 0;
            if (rand() % 2 == 0 && z < MAZE_HEIGHT - 8) maze[x][z+3] = 0; // A veces 4 de ancho
        }
    }
    
    // Pasillos verticales principales (muy anchos)
    for (int z = 5; z < MAZE_HEIGHT - 5; z += 8) {
        for (int x = 5; x < MAZE_WIDTH - 5; x++) {
            if (rand() % 2 == 0) { // 50% de probabilidad de pasillo vertical
                // Pasillo de 2-3 unidades de ancho
                maze[x][z] = 0;
                if (x < MAZE_WIDTH - 6) maze[x+1][z] = 0;
                if (rand() % 2 == 0 && x < MAZE_WIDTH - 7) maze[x+2][z] = 0; // A veces 3 de ancho
            }
        }
    }
    
    // Crear pasillos secundarios más estrechos
    for (int x = 2; x < MAZE_WIDTH - 2; x += 4) {
        for (int z = 2; z < MAZE_HEIGHT - 2; z++) {
            if (rand() % 3 == 0) { // 33% de probabilidad
                maze[x][z] = 0; // Pasillo de 1 unidad
                if (rand() % 2 == 0) maze[x][z+1] = 0; // A veces 2 de ancho
            }
        }
    }
}

void generate_room_maze() {
    // Crear salas grandes de diferentes tamaños (estilo backrooms)
    for (int room = 0; room < 20; room++) { // Más salas
        int roomX = rand() % (MAZE_WIDTH - 20) + 10;
        int roomZ = rand() % (MAZE_HEIGHT - 20) + 10;
        int roomW = rand() % 12 + 8; // 8-19 de ancho (salas más grandes)
        int roomH = rand() % 12 + 8; // 8-19 de alto (salas más grandes)
        
        // Asegurar que la habitación quepa
        if (roomX + roomW < MAZE_WIDTH - 2 && roomZ + roomH < MAZE_HEIGHT - 2) {
            for (int x = roomX; x < roomX + roomW; x++) {
                for (int z = roomZ; z < roomZ + roomH; z++) {
                    maze[x][z] = 0;
                }
            }
        }
    }
    
    // Crear salas medianas
    for (int room = 0; room < 20; room++) {
        int roomX = rand() % (MAZE_WIDTH - 8) + 3;
        int roomZ = rand() % (MAZE_HEIGHT - 8) + 3;
        int roomW = rand() % 4 + 3; // 3-6 de ancho
        int roomH = rand() % 4 + 3; // 3-6 de alto
        
        if (roomX + roomW < MAZE_WIDTH - 2 && roomZ + roomH < MAZE_HEIGHT - 2) {
                for (int x = roomX; x < roomX + roomW; x++) {
                for (int z = roomZ; z < roomZ + roomH; z++) {
                    maze[x][z] = 0;
                }
            }
        }
    }
    
    // Conectar habitaciones con pasillos de diferentes anchos
    for (int i = 0; i < 25; i++) {
        int x = rand() % (MAZE_WIDTH - 2) + 1;
        int z = rand() % (MAZE_HEIGHT - 2) + 1;
        maze[x][z] = 0;
        
        // A veces crear pasillos de conexión más anchos
        if (rand() % 3 == 0) {
            if (x < MAZE_WIDTH - 3) maze[x+1][z] = 0;
            if (z < MAZE_HEIGHT - 3) maze[x][z+1] = 0;
        }
    }
}

void generate_winding_corridors() {
    // Crear pasillos serpenteantes de diferentes anchos
    int startX = MAZE_WIDTH / 2;
    int startZ = MAZE_HEIGHT / 2;
    int currentX = startX;
    int currentZ = startZ;
    
    for (int steps = 0; steps < 300; steps++) {
        // Crear pasillo en la posición actual
        maze[currentX][currentZ] = 0;
        
        // A veces crear pasillos más anchos
        int corridor_width = 1;
        if (rand() % 4 == 0) corridor_width = 2; // 25% de probabilidad de pasillo doble
        if (rand() % 8 == 0) corridor_width = 3; // 12.5% de probabilidad de pasillo triple
        
        // Expandir el pasillo según el ancho
        for (int w = 1; w < corridor_width; w++) {
            if (currentX + w < MAZE_WIDTH - 1) maze[currentX + w][currentZ] = 0;
            if (currentZ + w < MAZE_HEIGHT - 1) maze[currentX][currentZ + w] = 0;
        }
        
        int direction = rand() % 4;
        switch (direction) {
            case 0: // Norte (Z negativo)
                if (currentZ > 1) currentZ--;
                break;
            case 1: // Sur (Z positivo)
                if (currentZ < MAZE_HEIGHT - 2) currentZ++;
                break;
            case 2: // Este (X positivo)
                if (currentX < MAZE_WIDTH - 2) currentX++;
                break;
            case 3: // Oeste (X negativo)
                if (currentX > 1) currentX--;
                break;
        }
    }
}

void generate_backrooms_pattern() {
    // Patrón auténtico de backrooms con pasillos de diferentes anchos
    
    // Crear grandes salas abiertas (como las típicas de backrooms)
    for (int room = 0; room < 15; room++) { // Más salas grandes
        int roomX = rand() % (MAZE_WIDTH - 25) + 12;
        int roomZ = rand() % (MAZE_HEIGHT - 25) + 12;
        int roomW = rand() % 15 + 10; // 10-24 de ancho (salas muy grandes)
        int roomH = rand() % 15 + 10; // 10-24 de alto (salas muy grandes)
        
        if (roomX + roomW < MAZE_WIDTH - 2 && roomZ + roomH < MAZE_HEIGHT - 2) {
            for (int x = roomX; x < roomX + roomW; x++) {
                for (int z = roomZ; z < roomZ + roomH; z++) {
                    maze[x][z] = 0;
                }
            }
        }
    }
    
    // Crear pasillos principales muy anchos (como los típicos de backrooms)
    for (int x = 3; x < MAZE_WIDTH - 3; x += 6) {
        for (int z = 3; z < MAZE_HEIGHT - 3; z++) {
            if (rand() % 2 == 0) { // 50% de probabilidad
                // Pasillo de 3-5 unidades de ancho
                maze[x][z] = 0;
                if (z < MAZE_HEIGHT - 4) maze[x][z+1] = 0;
                if (z < MAZE_HEIGHT - 5) maze[x][z+2] = 0;
                if (rand() % 2 == 0 && z < MAZE_HEIGHT - 6) maze[x][z+3] = 0;
                if (rand() % 3 == 0 && z < MAZE_HEIGHT - 7) maze[x][z+4] = 0;
            }
        }
    }
    
    // Crear pasillos secundarios de diferentes anchos
    for (int x = 2; x < MAZE_WIDTH - 2; x += 3) {
        for (int z = 2; z < MAZE_HEIGHT - 2; z++) {
            if (rand() % 4 == 0) { // 25% de probabilidad
                maze[x][z] = 0; // Pasillo base
                if (rand() % 2 == 0) maze[x][z+1] = 0; // A veces 2 de ancho
                if (rand() % 3 == 0) maze[x+1][z] = 0; // A veces también en X
            }
        }
    }
    
    // Crear callejones sin salida (muy típicos de backrooms)
    for (int i = 0; i < 40; i++) {
        int startX = rand() % (MAZE_WIDTH - 6) + 3;
        int startZ = rand() % (MAZE_HEIGHT - 6) + 3;
        
        if (maze[startX][startZ] == 0) { // Empezar desde un pasillo existente
            int length = rand() % 12 + 4; // Longitud del callejón 4-15
            int direction = rand() % 4;
            int width = rand() % 2 + 1; // Ancho 1-2
            
            for (int j = 0; j < length; j++) {
                switch (direction) {
                    case 0: // Norte
                        if (startZ - j > 0) {
                            maze[startX][startZ - j] = 0;
                            if (width == 2 && startX + 1 < MAZE_WIDTH - 1) 
                                maze[startX + 1][startZ - j] = 0;
                        }
                        break;
                    case 1: // Sur
                        if (startZ + j < MAZE_HEIGHT - 1) {
                            maze[startX][startZ + j] = 0;
                            if (width == 2 && startX + 1 < MAZE_WIDTH - 1) 
                                maze[startX + 1][startZ + j] = 0;
                        }
                        break;
                    case 2: // Este
                        if (startX + j < MAZE_WIDTH - 1) {
                            maze[startX + j][startZ] = 0;
                            if (width == 2 && startZ + 1 < MAZE_HEIGHT - 1) 
                                maze[startX + j][startZ + 1] = 0;
                        }
                        break;
                    case 3: // Oeste
                        if (startX - j > 0) {
                            maze[startX - j][startZ] = 0;
                            if (width == 2 && startZ + 1 < MAZE_HEIGHT - 1) 
                                maze[startX - j][startZ + 1] = 0;
                        }
                        break;
                }
            }
        }
    }
    
    // Crear columnas sueltas de diferentes tamaños (típicas de backrooms)
    for (int i = 0; i < 80; i++) { // Muchas más columnas
        int x = rand() % (MAZE_WIDTH - 6) + 3;
        int z = rand() % (MAZE_HEIGHT - 6) + 3;
        
        // Solo crear columnas en espacios abiertos
        if (maze[x][z] == 0) {
            int columnType = rand() % 4; // 4 tipos de columnas
            
            switch (columnType) {
                case 0: // Columna simple 1x1
                    maze[x][z] = 1;
                    break;
                case 1: // Columna 2x2
                    if (x < MAZE_WIDTH - 2 && z < MAZE_HEIGHT - 2) {
                        maze[x][z] = 1;
                        maze[x+1][z] = 1;
                        maze[x][z+1] = 1;
                        maze[x+1][z+1] = 1;
                    }
                    break;
                case 2: // Columna 3x3
                    if (x < MAZE_WIDTH - 3 && z < MAZE_HEIGHT - 3) {
                        for (int dx = 0; dx < 3; dx++) {
                            for (int dz = 0; dz < 3; dz++) {
                                maze[x+dx][z+dz] = 1;
                            }
                        }
                    }
                    break;
                case 3: // Columna rectangular 2x1 o 1x2
                    if (rand() % 2 == 0) { // 2x1
                        if (x < MAZE_WIDTH - 2) {
                            maze[x][z] = 1;
                            maze[x+1][z] = 1;
                        }
                    } else { // 1x2
                        if (z < MAZE_HEIGHT - 2) {
                            maze[x][z] = 1;
                            maze[x][z+1] = 1;
                        }
                    }
                    break;
            }
        }
    }
}

void ensure_single_exit() {
    // Seleccionar UN SOLO borde aleatoriamente para la salida
    exit_side = rand() % 4; // 0=Norte, 1=Sur, 2=Este, 3=Oeste
    exit_pos = 0;
    
    switch (exit_side) {
        case 0: // Norte (Z=0)
            exit_pos = rand() % (MAZE_WIDTH - 4) + 2; // Evitar esquinas
            maze[exit_pos][0] = 0;
            maze[exit_pos][1] = 0;
            maze[exit_pos][2] = 0; // Hacer la salida más ancha
            break;
        case 1: // Sur (Z=MAZE_HEIGHT-1)
            exit_pos = rand() % (MAZE_WIDTH - 4) + 2;
            maze[exit_pos][MAZE_HEIGHT - 1] = 0;
            maze[exit_pos][MAZE_HEIGHT - 2] = 0;
            maze[exit_pos][MAZE_HEIGHT - 3] = 0;
            break;
        case 2: // Este (X=MAZE_WIDTH-1)
            exit_pos = rand() % (MAZE_HEIGHT - 4) + 2;
            maze[MAZE_WIDTH - 1][exit_pos] = 0;
            maze[MAZE_WIDTH - 2][exit_pos] = 0;
            maze[MAZE_WIDTH - 3][exit_pos] = 0;
            break;
        case 3: // Oeste (X=0)
            exit_pos = rand() % (MAZE_HEIGHT - 4) + 2;
            maze[0][exit_pos] = 0;
            maze[1][exit_pos] = 0;
            maze[2][exit_pos] = 0;
            break;
    }
    
    // Crear un laberinto complejo que conecte la salida con el centro
    create_complex_path_to_exit(exit_side, exit_pos);
}

void create_main_corridors(int exits[4]) {
    // Pasillo principal norte-sur (Z)
    for (int z = 1; z < MAZE_HEIGHT - 1; z++) {
        if (rand() % 3 == 0) { // 33% de probabilidad
            maze[exits[0]][z] = 0; // Conectar con salida norte
            maze[exits[1]][z] = 0; // Conectar con salida sur
        }
    }
    
    // Pasillo principal este-oeste (X)
    for (int x = 1; x < MAZE_WIDTH - 1; x++) {
        if (rand() % 3 == 0) { // 33% de probabilidad
            maze[x][exits[2]] = 0; // Conectar con salida este
            maze[x][exits[3]] = 0; // Conectar con salida oeste
        }
    }
}

void connect_exits_to_center(int exits[4]) {
    // Conectar norte al centro
    for (int i = 0; i < 3; i++) {
        if (exits[0] > 1) maze[exits[0] - 1][1] = 0;
        if (exits[0] < MAZE_WIDTH - 2) maze[exits[0] + 1][1] = 0;
    }
    
    // Conectar sur al centro
    for (int i = 0; i < 3; i++) {
        if (exits[1] > 1) maze[exits[1] - 1][MAZE_HEIGHT - 2] = 0;
        if (exits[1] < MAZE_WIDTH - 2) maze[exits[1] + 1][MAZE_HEIGHT - 2] = 0;
    }
    
    // Conectar este al centro
    for (int i = 0; i < 3; i++) {
        if (exits[2] > 1) maze[MAZE_WIDTH - 2][exits[2] - 1] = 0;
        if (exits[2] < MAZE_HEIGHT - 2) maze[MAZE_WIDTH - 2][exits[2] + 1] = 0;
    }
    
    // Conectar oeste al centro
    for (int i = 0; i < 3; i++) {
        if (exits[3] > 1) maze[1][exits[3] - 1] = 0;
        if (exits[3] < MAZE_HEIGHT - 2) maze[1][exits[3] + 1] = 0;
    }
    
    // Crear pasillos principales hacia el centro desde cada salida
    create_main_corridors(exits);
}

void create_complex_path_to_exit(int exit_side, int exit_pos) {
    int centerX = MAZE_WIDTH / 2;
    int centerZ = MAZE_HEIGHT / 2;
    
    // Crear un camino serpenteante y complejo desde el centro hacia la salida
    int currentX = centerX;
    int currentZ = centerZ;
    int targetX = 0, targetZ = 0; // Inicializar las variables
    
    // Determinar la dirección hacia la salida
    switch (exit_side) {
        case 0: // Norte (Z=0)
            targetX = exit_pos;
            targetZ = 0;
            break;
        case 1: // Sur (Z=MAZE_HEIGHT-1)
            targetX = exit_pos;
            targetZ = MAZE_HEIGHT - 1;
            break;
        case 2: // Este (X=MAZE_WIDTH-1)
            targetX = MAZE_WIDTH - 1;
            targetZ = exit_pos;
            break;
        case 3: // Oeste (X=0)
            targetX = 0;
            targetZ = exit_pos;
            break;
    }
    
    // Crear camino principal con desvíos y callejones sin salida
    for (int step = 0; step < 200; step++) {
        // Asegurar que el camino actual esté libre
        maze[currentX][currentZ] = 0;
        
        // Calcular dirección hacia el objetivo
        int dirX = (targetX > currentX) ? 1 : (targetX < currentX) ? -1 : 0;
        int dirZ = (targetZ > currentZ) ? 1 : (targetZ < currentZ) ? -1 : 0;
        
        // A veces tomar un desvío aleatorio para hacer el camino más complejo
        if (rand() % 4 == 0) {
            dirX = (rand() % 3) - 1; // -1, 0, o 1
            dirZ = (rand() % 3) - 1;
        }
        
        // Mover hacia la dirección calculada
        int newX = currentX + dirX;
        int newZ = currentZ + dirZ;
        
        // Verificar límites
        if (newX > 0 && newX < MAZE_WIDTH - 1 && newZ > 0 && newZ < MAZE_HEIGHT - 1) {
            currentX = newX;
            currentZ = newZ;
        }
        
        // Si llegamos cerca del objetivo, terminar
        if (abs(currentX - targetX) <= 2 && abs(currentZ - targetZ) <= 2) {
            break;
        }
    }
    
    // Crear callejones sin salida para aumentar la dificultad
    create_dead_ends();
}

void create_dead_ends() {
    // Crear callejones sin salida de diferentes anchos (típicos de backrooms)
    for (int i = 0; i < 50; i++) {
        int startX = rand() % (MAZE_WIDTH - 6) + 3;
        int startZ = rand() % (MAZE_HEIGHT - 6) + 3;
        
        if (maze[startX][startZ] == 0) { // Empezar desde un pasillo existente
            int length = rand() % 12 + 4; // Longitud del callejón 4-15
            int direction = rand() % 4;
            int width = rand() % 3 + 1; // Ancho 1-3 (más variado)
            
            for (int j = 0; j < length; j++) {
                switch (direction) {
                    case 0: // Norte (Z negativo)
                        if (startZ - j > 0) {
                            maze[startX][startZ - j] = 0;
                            // Expandir según el ancho
                            for (int w = 1; w < width; w++) {
                                if (startX + w < MAZE_WIDTH - 1) 
                                    maze[startX + w][startZ - j] = 0;
                            }
                        }
                        break;
                    case 1: // Sur (Z positivo)
                        if (startZ + j < MAZE_HEIGHT - 1) {
                            maze[startX][startZ + j] = 0;
                            // Expandir según el ancho
                            for (int w = 1; w < width; w++) {
                                if (startX + w < MAZE_WIDTH - 1) 
                                    maze[startX + w][startZ + j] = 0;
                            }
                        }
                        break;
                    case 2: // Este (X positivo)
                        if (startX + j < MAZE_WIDTH - 1) {
                            maze[startX + j][startZ] = 0;
                            // Expandir según el ancho
                            for (int w = 1; w < width; w++) {
                                if (startZ + w < MAZE_HEIGHT - 1) 
                                    maze[startX + j][startZ + w] = 0;
                            }
                        }
                        break;
                    case 3: // Oeste (X negativo)
                        if (startX - j > 0) {
                            maze[startX - j][startZ] = 0;
                            // Expandir según el ancho
                            for (int w = 1; w < width; w++) {
                                if (startZ + w < MAZE_HEIGHT - 1) 
                                    maze[startX - j][startZ + w] = 0;
                            }
                        }
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
        int z = rand() % (MAZE_HEIGHT - 2) + 1;
        if (maze[x][z] == 0) { // Solo en espacios vacíos
            // Crear pequeñas estructuras decorativas
            if (rand() % 3 == 0) {
                maze[x][z] = 2; // Marcar como elemento decorativo
            }
        }
    }
}

bool is_wall(int x, int z) {
    if (x < 0 || x >= MAZE_WIDTH || z < 0 || z >= MAZE_HEIGHT) {
        return true; // Fuera del mapa = pared
    }
    return maze[x][z] == 1;
}

void create_guaranteed_path_to_exit() {
    // Crear un camino directo y garantizado desde el centro hasta la salida
    int centerX = MAZE_WIDTH / 2;
    int centerZ = MAZE_HEIGHT / 2;
    
    // Si no hay salida definida, crear una
    if (exit_side == -1) {
        ensure_single_exit();
    }
    
    // Crear camino directo desde el centro hacia la salida
    int currentX = centerX;
    int currentZ = centerZ;
    int targetX = 0, targetZ = 0;
    
    // Determinar la dirección hacia la salida
    switch (exit_side) {
        case 0: // Norte (Z=0)
            targetX = exit_pos;
            targetZ = 0;
            break;
        case 1: // Sur (Z=MAZE_HEIGHT-1)
            targetX = exit_pos;
            targetZ = MAZE_HEIGHT - 1;
            break;
        case 2: // Este (X=MAZE_WIDTH-1)
            targetX = MAZE_WIDTH - 1;
            targetZ = exit_pos;
            break;
        case 3: // Oeste (X=0)
            targetX = 0;
            targetZ = exit_pos;
            break;
    }
    
    // Crear camino directo con algunos desvíos para hacerlo más interesante
    while (currentX != targetX || currentZ != targetZ) {
        // Asegurar que el camino actual esté libre
        maze[currentX][currentZ] = 0;
        
        // Calcular dirección hacia el objetivo
        int dirX = (targetX > currentX) ? 1 : (targetX < currentX) ? -1 : 0;
        int dirZ = (targetZ > currentZ) ? 1 : (targetZ < currentZ) ? -1 : 0;
        
        // A veces tomar un desvío pequeño para hacer el camino más interesante
        if (rand() % 8 == 0) {
            if (dirX == 0) dirX = (rand() % 3) - 1; // -1, 0, o 1
            if (dirZ == 0) dirZ = (rand() % 3) - 1;
        }
        
        // Mover hacia la dirección calculada
        int newX = currentX + dirX;
        int newZ = currentZ + dirZ;
        
        // Verificar límites
        if (newX >= 0 && newX < MAZE_WIDTH && newZ >= 0 && newZ < MAZE_HEIGHT) {
            currentX = newX;
            currentZ = newZ;
        } else {
            // Si no podemos movernos, tomar una dirección alternativa
            if (dirX != 0) {
                currentX += dirX;
            } else if (dirZ != 0) {
                currentZ += dirZ;
            } else {
                break; // No podemos movernos más
            }
        }
        
        // Evitar bucle infinito
        static int steps = 0;
        if (++steps > MAZE_WIDTH + MAZE_HEIGHT) break;
    }
}

void ensure_connectivity() {
    // Verificar si el centro está conectado a la salida
    int centerX = MAZE_WIDTH / 2;
    int centerZ = MAZE_HEIGHT / 2;
    
    if (!is_connected_to_exit(centerX, centerZ)) {
        // Si no está conectado, crear un camino de emergencia
        create_guaranteed_path_to_exit();
    }
}

bool is_connected_to_exit(int startX, int startZ) {
    // Usar flood fill para verificar conectividad
    bool visited[MAZE_WIDTH][MAZE_HEIGHT];
    
    // Inicializar array de visitados
    for (int x = 0; x < MAZE_WIDTH; x++) {
        for (int z = 0; z < MAZE_HEIGHT; z++) {
            visited[x][z] = false;
        }
    }
    
    // Hacer flood fill desde el punto de inicio
    flood_fill_connectivity(startX, startZ, visited);
    
    // Verificar si la salida fue visitada
    switch (exit_side) {
        case 0: // Norte
            return visited[exit_pos][0];
        case 1: // Sur
            return visited[exit_pos][MAZE_HEIGHT - 1];
        case 2: // Este
            return visited[MAZE_WIDTH - 1][exit_pos];
        case 3: // Oeste
            return visited[0][exit_pos];
        default:
            return false;
    }
}

void flood_fill_connectivity(int x, int z, bool visited[MAZE_WIDTH][MAZE_HEIGHT]) {
    // Verificar límites
    if (x < 0 || x >= MAZE_WIDTH || z < 0 || z >= MAZE_HEIGHT) return;
    
    // Si ya fue visitado o es una pared, no continuar
    if (visited[x][z] || maze[x][z] == 1) return;
    
    // Marcar como visitado
    visited[x][z] = true;
    
    // Continuar con las 4 direcciones
    flood_fill_connectivity(x + 1, z, visited); // Este
    flood_fill_connectivity(x - 1, z, visited); // Oeste
    flood_fill_connectivity(x, z + 1, visited); // Sur
    flood_fill_connectivity(x, z - 1, visited); // Norte
}

// ===== NUEVO SISTEMA DE GENERACIÓN AVANZADA =====

void generate_advanced_backrooms() {
    // Crear red de salas grandes y realistas
    create_room_network();
    
    // Generar pasillos anchos que conecten las salas
    generate_wide_corridors();
    
    // Conectar todas las salas con pasillos
    connect_rooms_with_corridors();
    
    // Colocar columnas sueltas de diferentes tamaños
    place_standalone_columns();
    
    // Generar puntos de luz como guías
    generate_light_points();
    
    // Aplicar todas las estructuras al mapa
    apply_structures_to_maze();
}

void create_room_network() {
    // Crear salas grandes distribuidas por todo el mapa
    for (int i = 0; i < 25; i++) {
        if (roomCount >= 100) break;
        
        Room newRoom;
        newRoom.x = rand() % (MAZE_WIDTH - 30) + 15;
        newRoom.z = rand() % (MAZE_HEIGHT - 30) + 15;
        newRoom.width = rand() % 20 + 15;  // 15-34 de ancho (muy grandes)
        newRoom.height = rand() % 20 + 15; // 15-34 de alto (muy grandes)
        newRoom.type = 0; // Sala
        newRoom.connected = false;
        
        // Verificar que no se superponga con salas existentes
        bool canPlace = true;
        for (int j = 0; j < roomCount; j++) {
            if (rooms[j].x < newRoom.x + newRoom.width + 5 &&
                rooms[j].x + rooms[j].width + 5 > newRoom.x &&
                rooms[j].z < newRoom.z + newRoom.height + 5 &&
                rooms[j].z + rooms[j].height + 5 > newRoom.z) {
                canPlace = false;
                break;
            }
        }
        
        if (canPlace && newRoom.x + newRoom.width < MAZE_WIDTH - 2 && 
            newRoom.z + newRoom.height < MAZE_HEIGHT - 2) {
            rooms[roomCount] = newRoom;
            roomCount++;
        }
    }
}

void generate_wide_corridors() {
    // Crear pasillos principales muy anchos
    for (int i = 0; i < 15; i++) {
        if (corridorCount >= 200) break;
        
        Corridor newCorridor;
        newCorridor.x1 = rand() % (MAZE_WIDTH - 20) + 10;
        newCorridor.z1 = rand() % (MAZE_HEIGHT - 20) + 10;
        newCorridor.x2 = newCorridor.x1 + (rand() % 40 - 20); // -20 a +20
        newCorridor.z2 = newCorridor.z1 + (rand() % 40 - 20); // -20 a +20
        newCorridor.width = rand() % 6 + 4; // 4-9 de ancho (muy anchos)
        newCorridor.isMain = (rand() % 3 == 0); // 33% son principales
        
        // Asegurar que esté dentro de los límites
        if (newCorridor.x2 < 0) newCorridor.x2 = 0;
        if (newCorridor.x2 >= MAZE_WIDTH) newCorridor.x2 = MAZE_WIDTH - 1;
        if (newCorridor.z2 < 0) newCorridor.z2 = 0;
        if (newCorridor.z2 >= MAZE_HEIGHT) newCorridor.z2 = MAZE_HEIGHT - 1;
        
        corridors[corridorCount] = newCorridor;
        corridorCount++;
    }
}

void connect_rooms_with_corridors() {
    // Conectar cada sala con múltiples salas para evitar callejones sin salida
    for (int i = 0; i < roomCount; i++) {
        // Conectar cada sala con 2-3 salas cercanas
        int connectionsNeeded = 2 + (rand() % 2); // 2-3 conexiones por sala
        int connectionsMade = 0;
        
        // Ordenar salas por distancia
        int sortedRooms[100];
        float distances[100];
        for (int j = 0; j < roomCount; j++) {
            if (i == j) {
                distances[j] = 999999.0f;
                continue;
            }
            distances[j] = sqrt((rooms[i].x - rooms[j].x) * (rooms[i].x - rooms[j].x) + 
                               (rooms[i].z - rooms[j].z) * (rooms[i].z - rooms[j].z));
            sortedRooms[j] = j;
        }
        
        // Ordenar por distancia (bubble sort simple)
        for (int a = 0; a < roomCount - 1; a++) {
            for (int b = 0; b < roomCount - 1 - a; b++) {
                if (distances[b] > distances[b + 1]) {
                    float tempDist = distances[b];
                    int tempRoom = sortedRooms[b];
                    distances[b] = distances[b + 1];
                    sortedRooms[b] = sortedRooms[b + 1];
                    distances[b + 1] = tempDist;
                    sortedRooms[b + 1] = tempRoom;
                }
            }
        }
        
        // Conectar con las salas más cercanas
        for (int k = 0; k < roomCount && connectionsMade < connectionsNeeded; k++) {
            int targetRoom = sortedRooms[k];
            if (targetRoom == i) continue;
            
            if (corridorCount < 200) {
                // Crear pasillo de conexión
                Corridor connection;
                connection.x1 = rooms[i].x + rooms[i].width / 2;
                connection.z1 = rooms[i].z + rooms[i].height / 2;
                connection.x2 = rooms[targetRoom].x + rooms[targetRoom].width / 2;
                connection.z2 = rooms[targetRoom].z + rooms[targetRoom].height / 2;
                connection.width = rand() % 4 + 4; // 4-7 de ancho (más anchos)
                connection.isMain = (connectionsMade == 0); // La primera conexión es principal
                
                corridors[corridorCount] = connection;
                corridorCount++;
                connectionsMade++;
                
                rooms[i].connected = true;
                rooms[targetRoom].connected = true;
            }
        }
    }
    
    // Crear pasillos adicionales para garantizar conectividad
    create_additional_connectivity();
}

void place_standalone_columns() {
    // Colocar columnas sueltas de diferentes tamaños
    for (int i = 0; i < 100; i++) {
        if (columnCount >= 150) break;
        
        Column newColumn;
        newColumn.x = rand() % (MAZE_WIDTH - 10) + 5;
        newColumn.z = rand() % (MAZE_HEIGHT - 10) + 5;
        newColumn.size = rand() % 3 + 1; // 1-3 de tamaño
        newColumn.type = rand() % 3; // 0 = columna, 1 = pilar, 2 = obstáculo
        
        columns[columnCount] = newColumn;
        columnCount++;
    }
}

void apply_structures_to_maze() {
    // Aplicar todas las salas al mapa
    for (int i = 0; i < roomCount; i++) {
        apply_room_to_maze(rooms[i]);
    }
    
    // Aplicar todos los pasillos al mapa
    for (int i = 0; i < corridorCount; i++) {
        apply_corridor_to_maze(corridors[i]);
    }
    
    // Aplicar todas las columnas al mapa
    for (int i = 0; i < columnCount; i++) {
        apply_column_to_maze(columns[i]);
    }
}

void apply_room_to_maze(Room room) {
    for (int x = room.x; x < room.x + room.width && x < MAZE_WIDTH; x++) {
        for (int z = room.z; z < room.z + room.height && z < MAZE_HEIGHT; z++) {
            if (x >= 0 && z >= 0) {
                maze[x][z] = 0; // Espacio libre
            }
        }
    }
}

void apply_corridor_to_maze(Corridor corridor) {
    // Crear pasillo entre dos puntos
    int dx = abs(corridor.x2 - corridor.x1);
    int dz = abs(corridor.z2 - corridor.z1);
    int steps = (dx > dz) ? dx : dz;
    
    if (steps == 0) return;
    
    for (int i = 0; i <= steps; i++) {
        int x = corridor.x1 + (corridor.x2 - corridor.x1) * i / steps;
        int z = corridor.z1 + (corridor.z2 - corridor.z1) * i / steps;
        
        // Aplicar el ancho del pasillo
        for (int w = 0; w < corridor.width; w++) {
            for (int h = 0; h < corridor.width; h++) {
                int px = x + w - corridor.width / 2;
                int pz = z + h - corridor.width / 2;
                
                if (px >= 0 && px < MAZE_WIDTH && pz >= 0 && pz < MAZE_HEIGHT) {
                    maze[px][pz] = 0; // Espacio libre
                }
            }
        }
    }
}

void apply_column_to_maze(Column column) {
    // Solo colocar columnas en espacios abiertos
    if (maze[column.x][column.z] == 0) {
        for (int dx = 0; dx < column.size; dx++) {
            for (int dz = 0; dz < column.size; dz++) {
                int px = column.x + dx;
                int pz = column.z + dz;
                
                if (px < MAZE_WIDTH && pz < MAZE_HEIGHT) {
                    maze[px][pz] = 1; // Columna (pared)
                }
            }
        }
    }
}

void create_additional_connectivity() {
    // Crear pasillos adicionales para garantizar múltiples caminos
    for (int i = 0; i < 20; i++) {
        if (corridorCount >= 200) break;
        
        // Crear pasillos aleatorios que conecten áreas distantes
        Corridor additional;
        additional.x1 = rand() % (MAZE_WIDTH - 20) + 10;
        additional.z1 = rand() % (MAZE_HEIGHT - 20) + 10;
        additional.x2 = rand() % (MAZE_WIDTH - 20) + 10;
        additional.z2 = rand() % (MAZE_HEIGHT - 20) + 10;
        additional.width = rand() % 5 + 3; // 3-7 de ancho
        additional.isMain = false;
        
        corridors[corridorCount] = additional;
        corridorCount++;
    }
    
    // Crear pasillos de respaldo que conecten el centro con los bordes
    int centerX = MAZE_WIDTH / 2;
    int centerZ = MAZE_HEIGHT / 2;
    
    // Conectar centro con cada borde
    for (int side = 0; side < 4; side++) {
        if (corridorCount >= 200) break;
        
        Corridor backup;
        backup.x1 = centerX;
        backup.z1 = centerZ;
        backup.width = 5; // Pasillo de respaldo ancho
        backup.isMain = true;
        
        switch (side) {
            case 0: // Norte
                backup.x2 = rand() % (MAZE_WIDTH - 10) + 5;
                backup.z2 = 5;
                break;
            case 1: // Sur
                backup.x2 = rand() % (MAZE_WIDTH - 10) + 5;
                backup.z2 = MAZE_HEIGHT - 5;
                break;
            case 2: // Este
                backup.x2 = MAZE_WIDTH - 5;
                backup.z2 = rand() % (MAZE_HEIGHT - 10) + 5;
                break;
            case 3: // Oeste
                backup.x2 = 5;
                backup.z2 = rand() % (MAZE_HEIGHT - 10) + 5;
                break;
        }
        
        corridors[corridorCount] = backup;
        corridorCount++;
    }
}

// ===== SISTEMA DE PUNTOS DE LUZ =====

void generate_light_points() {
    // Colocar luces en salas grandes
    for (int i = 0; i < roomCount && lightCount < 50; i++) {
        if (rooms[i].width > 20 && rooms[i].height > 20) { // Solo en salas muy grandes
            place_light_in_room(rooms[i]);
        }
    }
    
    // Colocar luces en pasillos principales
    for (int i = 0; i < corridorCount && lightCount < 50; i++) {
        if (corridors[i].isMain && corridors[i].width > 5) { // Solo en pasillos principales anchos
            place_light_in_corridor(corridors[i]);
        }
    }
    
    // Colocar luces aleatorias en espacios abiertos
    for (int i = 0; i < 15 && lightCount < 50; i++) {
        int x = rand() % (MAZE_WIDTH - 10) + 5;
        int z = rand() % (MAZE_HEIGHT - 10) + 5;
        
        // Solo en espacios abiertos
        if (maze[x][z] == 0) {
            LightPoint newLight;
            newLight.x = (float)x + 0.5f;
            newLight.z = (float)z + 0.5f;
            newLight.type = rand() % 3; // 0 = tenue, 1 = normal, 2 = brillante
            
            switch (newLight.type) {
                case 0: // Luz tenue
                    newLight.intensity = 0.3f;
                    newLight.range = 8.0f;
                    break;
                case 1: // Luz normal
                    newLight.intensity = 0.6f;
                    newLight.range = 12.0f;
                    break;
                case 2: // Luz brillante
                    newLight.intensity = 0.9f;
                    newLight.range = 16.0f;
                    break;
            }
            
            newLight.active = true;
            lightPoints[lightCount] = newLight;
            lightCount++;
        }
    }
}

void place_light_in_room(Room room) {
    if (lightCount >= 50) return;
    
    // Colocar 1-2 luces por sala grande
    int lightsInRoom = 1 + (rand() % 2); // 1-2 luces
    
    for (int i = 0; i < lightsInRoom && lightCount < 50; i++) {
        LightPoint newLight;
        
        // Posición aleatoria dentro de la sala
        newLight.x = room.x + (rand() % (room.width - 4)) + 2.0f;
        newLight.z = room.z + (rand() % (room.height - 4)) + 2.0f;
        
        // Tipo de luz basado en el tamaño de la sala
        if (room.width > 30 && room.height > 30) {
            newLight.type = 2; // Luz brillante para salas muy grandes
            newLight.intensity = 0.8f;
            newLight.range = 18.0f;
        } else {
            newLight.type = 1; // Luz normal para salas grandes
            newLight.intensity = 0.6f;
            newLight.range = 14.0f;
        }
        
        newLight.active = true;
        lightPoints[lightCount] = newLight;
        lightCount++;
    }
}

void place_light_in_corridor(Corridor corridor) {
    if (lightCount >= 50) return;
    
    // Colocar luces a lo largo del pasillo
    int dx = abs(corridor.x2 - corridor.x1);
    int dz = abs(corridor.z2 - corridor.z1);
    int steps = (dx > dz) ? dx : dz;
    
    if (steps < 10) return; // Solo en pasillos largos
    
    // Colocar 2-4 luces a lo largo del pasillo
    int lightsInCorridor = 2 + (rand() % 3); // 2-4 luces
    
    for (int i = 0; i < lightsInCorridor && lightCount < 50; i++) {
        LightPoint newLight;
        
        // Posición a lo largo del pasillo
        float t = (float)i / (float)(lightsInCorridor - 1);
        newLight.x = corridor.x1 + (corridor.x2 - corridor.x1) * t;
        newLight.z = corridor.z1 + (corridor.z2 - corridor.z1) * t;
        
        // Luz tenue para pasillos
        newLight.type = 0;
        newLight.intensity = 0.4f;
        newLight.range = 10.0f;
        newLight.active = true;
        
        lightPoints[lightCount] = newLight;
        lightCount++;
    }
}


void cleanup_map() {
    // Limpiar recursos del mapa (si los hay)
    exit_side = -1;
    exit_pos = -1;
    roomCount = 0;
    corridorCount = 0;
    columnCount = 0;
    lightCount = 0;
}
