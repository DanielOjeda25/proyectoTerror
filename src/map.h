// map.h - Sistema de mapas para Backrooms 3D
#ifndef MAP_H
#define MAP_H

#include <stdbool.h>

// Constantes del mapa
#define MAZE_WIDTH 200
#define MAZE_HEIGHT 200

// Variables globales del mapa
extern int maze[MAZE_HEIGHT][MAZE_WIDTH];

// Funciones del mapa
void init_map();
void generate_map();
bool is_wall(int x, int z);
void cleanup_map();

// Funciones de generación de patrones
void generate_classic_maze();
void generate_room_maze();
void generate_winding_corridors();
void generate_backrooms_pattern();
void ensure_single_exit();
void create_complex_path_to_exit(int exit_side, int exit_pos);
void create_dead_ends();
void create_main_corridors(int exits[4]);
void add_decorative_elements();

#endif // MAP_H
