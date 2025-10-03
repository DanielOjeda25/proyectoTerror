// audio.c - Sistema de audio simplificado para Backrooms
#include "audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

// Variables para control de sonidos
static int current_footstep = 0;
static int current_running = 0;

// Variables para control de canales de audio
static BOOL ambient_playing = FALSE;
static BOOL footstep_playing = FALSE;
static BOOL running_playing = FALSE;

// Variables para temporizadores
static DWORD footstep_timer = 0;
static DWORD running_timer = 0;

void init_audio() {
    // Inicializar sistema de audio de Windows
    printf("Sistema de audio simplificado inicializado\n");
}

void play_footstep_sound() {
    DWORD current_time = GetTickCount();
    
    // Solo reproducir si ha pasado suficiente tiempo desde el último sonido
    if (footstep_playing && (current_time - footstep_timer) < 500) return;
    
    char filename[50];
    current_footstep = (current_footstep % 4) + 1;
    sprintf(filename, "sounds/footstep_%d.wav", current_footstep);
    
    printf("Reproduciendo: %s\n", filename);
    PlaySound(filename, NULL, SND_FILENAME | SND_ASYNC);
    
    footstep_playing = TRUE;
    footstep_timer = current_time;
}

void play_running_sound() {
    DWORD current_time = GetTickCount();
    
    // Solo reproducir si ha pasado suficiente tiempo desde el último sonido
    if (running_playing && (current_time - running_timer) < 400) return;
    
    char filename[50];
    current_running = (current_running % 3) + 1;
    sprintf(filename, "sounds/running_%d.wav", current_running);
    
    printf("Reproduciendo: %s\n", filename);
    PlaySound(filename, NULL, SND_FILENAME | SND_ASYNC);
    
    running_playing = TRUE;
    running_timer = current_time;
}

void play_ambient_sound() {
    // Solo iniciar el ambiental si no está reproduciéndose
    if (ambient_playing) return;
    
    printf("Iniciando sonido ambiental...\n");
    PlaySound("sounds/ambient.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    ambient_playing = TRUE;
}

void play_enemy_sound() {
    printf("Reproduciendo sonido de enemigo...\n");
    PlaySound("sounds/enemy.wav", NULL, SND_FILENAME | SND_ASYNC);
}

void play_death_sound() {
    printf("Reproduciendo sonido de muerte...\n");
    PlaySound("sounds/death.wav", NULL, SND_FILENAME | SND_ASYNC);
}

void update_audio() {
    // Actualizar estado de los sonidos basado en el tiempo
    DWORD current_time = GetTickCount();
    
    // Resetear flags de sonidos que ya terminaron
    if (footstep_playing && (current_time - footstep_timer) >= 500) {
        footstep_playing = FALSE;
    }
    
    if (running_playing && (current_time - running_timer) >= 400) {
        running_playing = FALSE;
    }
}

void cleanup_audio() {
    // Detener todos los sonidos
    printf("Deteniendo todos los sonidos...\n");
    PlaySound(NULL, NULL, 0);
    ambient_playing = FALSE;
    footstep_playing = FALSE;
    running_playing = FALSE;
    printf("Sistema de audio limpiado\n");
}

// Funciones de hilos (no usadas en el sistema simplificado)
DWORD WINAPI ambient_thread(LPVOID lpParam) {
    return 0;
}

DWORD WINAPI footstep_thread(LPVOID lpParam) {
    return 0;
}

DWORD WINAPI running_thread(LPVOID lpParam) {
    return 0;
}