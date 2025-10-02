// audio.c - Sistema simple de audio para Windows
#include "audio.h"
#include <stdio.h>
#include <stdlib.h>

// Variables para control de sonidos
static int current_footstep = 0;
static int current_running = 0;

void init_audio() {
    // Inicializar sistema de audio de Windows
    printf("Sistema de audio inicializado\n");
}

void play_footstep_sound() {
    // Reproducir sonido de pasos generado
    char filename[50];
    current_footstep = (current_footstep % 4) + 1; // Ciclar entre 1-4
    sprintf(filename, "sounds/footstep_%d.wav", current_footstep);
    
    PlaySound(filename, NULL, SND_FILENAME | SND_ASYNC);
}

void play_running_sound() {
    // Reproducir sonido de correr generado
    char filename[50];
    current_running = (current_running % 3) + 1; // Ciclar entre 1-3
    sprintf(filename, "sounds/running_%d.wav", current_running);
    
    PlaySound(filename, NULL, SND_FILENAME | SND_ASYNC);
}

void play_ambient_sound() {
    // Reproducir sonido ambiental generado
    PlaySound("sounds/ambient.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

void cleanup_audio() {
    // Detener todos los sonidos
    PlaySound(NULL, NULL, 0);
    printf("Sistema de audio limpiado\n");
}
