// map.h - Sistema de mapas para Backrooms 3D
#ifndef MAP_H
#define MAP_H

#include <stdbool.h>

// Constantes del mapa
#define MAZE_WIDTH 25
#define MAZE_HEIGHT 25

// Variables globales del mapa
extern int maze[MAZE_HEIGHT][MAZE_WIDTH];

// Funciones del mapa
void init_map();
void generate_map();
bool is_wall(int x, int z);
void cleanup_map();

#endif // MAP_H
