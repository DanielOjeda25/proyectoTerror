// map.h - Sistema de mapas para Backrooms 3D
#ifndef MAP_H
#define MAP_H

#include <stdbool.h>

// Constantes del mapa - REDUCIDAS PARA MEJOR RENDIMIENTO
#define MAZE_WIDTH 100
#define MAZE_HEIGHT 100
#define MAZE_LEVELS 15  // Número de niveles de altura (reducido para mejor rendimiento)

// Sistema de renderizado optimizado
#define RENDER_DISTANCE 30.0f    // Distancia de renderizado en unidades

// Estructuras de datos avanzadas para mapas más realistas
typedef struct {
    int x, z;
    int width, height;
    int type; // 0 = sala, 1 = pasillo, 2 = intersección
    bool connected;
} Room;

typedef struct {
    int x1, z1, x2, z2;
    int width;
    bool isMain; // Si es un pasillo principal
} Corridor;

typedef struct {
    int x, z;
    int size; // 1 = 1x1, 2 = 2x2, 3 = 3x3
    int type; // 0 = columna, 1 = pilar, 2 = obstáculo
} Column;

typedef struct {
    float x, z;
    float intensity; // Intensidad de la luz (0.0 - 1.0)
    float range; // Rango de la luz
    bool active; // Si la luz está activa
    int type; // 0 = luz tenue, 1 = luz normal, 2 = luz brillante
} LightPoint;

// Variables globales del mapa
extern int maze[MAZE_WIDTH][MAZE_HEIGHT];
extern Room rooms[100]; // Hasta 100 salas
extern Corridor corridors[200]; // Hasta 200 pasillos
extern Column columns[150]; // Hasta 150 columnas
extern LightPoint lightPoints[50]; // Hasta 50 puntos de luz
extern int roomCount;
extern int corridorCount;
extern int columnCount;
extern int lightCount;

// Variables de renderizado optimizado
extern bool map_preloaded;
extern bool map_generation_complete;

// Funciones del mapa
void init_map();
void generate_map();
bool is_wall(int x, int z);
void cleanup_map();

// Sistema de precarga
void preload_map();
bool is_map_ready();
void show_loading_progress();

// Sistema de renderizado optimizado

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

// Nuevas funciones para garantizar conectividad
void create_guaranteed_path_to_exit();
void ensure_connectivity();
bool is_connected_to_exit(int startX, int startZ);
void flood_fill_connectivity(int x, int z, bool visited[MAZE_WIDTH][MAZE_HEIGHT]);

// Funciones de generación avanzada con estructuras de datos mejoradas
void generate_advanced_backrooms();
void create_room_network();
void generate_wide_corridors();
void place_standalone_columns();
void connect_rooms_with_corridors();
void apply_room_to_maze(Room room);
void apply_corridor_to_maze(Corridor corridor);
void apply_column_to_maze(Column column);
void apply_structures_to_maze();
void create_additional_connectivity();
void generate_light_points();
void place_light_in_room(Room room);
void place_light_in_corridor(Corridor corridor);

// Función para detectar si el jugador llegó a la salida
bool check_exit_reached(float x, float z);

#endif // MAP_H
