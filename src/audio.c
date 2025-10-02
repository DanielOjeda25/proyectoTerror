// audio.c - Sistema multi-hilo de audio para Backrooms
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

// Variables para hilos
static HANDLE ambient_thread_handle = NULL;
static HANDLE footstep_thread_handle = NULL;
static HANDLE running_thread_handle = NULL;
static BOOL ambient_thread_active = FALSE;
static BOOL footstep_thread_active = FALSE;
static BOOL running_thread_active = FALSE;

// Variables para control de hilos
static BOOL ambient_should_stop = FALSE;
static BOOL footstep_should_stop = FALSE;
static BOOL running_should_stop = FALSE;

void init_audio() {
    // Inicializar sistema de audio de Windows
    printf("Sistema de audio multi-hilo inicializado\n");
}

// Hilo para sonido ambiental (reproduce en loop)
DWORD WINAPI ambient_thread(LPVOID lpParam) {
    while (!ambient_should_stop) {
        if (ambient_playing && !ambient_should_stop) {
            PlaySound("sounds/ambient.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
            // Esperar un poco antes de verificar de nuevo
            Sleep(100);
        } else {
            Sleep(50);
        }
    }
    return 0;
}

// Hilo para sonidos de pisadas
DWORD WINAPI footstep_thread(LPVOID lpParam) {
    while (!footstep_should_stop) {
        if (footstep_playing && !footstep_should_stop) {
            char filename[50];
            current_footstep = (current_footstep % 4) + 1;
            sprintf(filename, "sounds/footstep_%d.wav", current_footstep);
            
            PlaySound(filename, NULL, SND_FILENAME | SND_ASYNC);
            
            // Esperar a que termine el sonido
            Sleep(500);
            footstep_playing = FALSE;
        } else {
            Sleep(50);
        }
    }
    return 0;
}

// Hilo para sonidos de correr
DWORD WINAPI running_thread(LPVOID lpParam) {
    while (!running_should_stop) {
        if (running_playing && !running_should_stop) {
            char filename[50];
            current_running = (current_running % 3) + 1;
            sprintf(filename, "sounds/running_%d.wav", current_running);
            
            PlaySound(filename, NULL, SND_FILENAME | SND_ASYNC);
            
            // Esperar a que termine el sonido
            Sleep(400);
            running_playing = FALSE;
        } else {
            Sleep(50);
        }
    }
    return 0;
}

void play_footstep_sound() {
    DWORD current_time = GetTickCount();
    
    // Solo reproducir si ha pasado suficiente tiempo desde el último sonido
    if (footstep_playing && (current_time - footstep_timer) < 500) return;
    
    footstep_playing = TRUE;
    footstep_timer = current_time;
    
    // Crear hilo si no existe
    if (footstep_thread_handle == NULL) {
        footstep_thread_handle = CreateThread(NULL, 0, footstep_thread, NULL, 0, NULL);
    }
}

void play_running_sound() {
    DWORD current_time = GetTickCount();
    
    // Solo reproducir si ha pasado suficiente tiempo desde el último sonido
    if (running_playing && (current_time - running_timer) < 400) return;
    
    running_playing = TRUE;
    running_timer = current_time;
    
    // Crear hilo si no existe
    if (running_thread_handle == NULL) {
        running_thread_handle = CreateThread(NULL, 0, running_thread, NULL, 0, NULL);
    }
}

void play_ambient_sound() {
    // Solo iniciar el ambiental si no está reproduciéndose
    if (ambient_playing) return;
    
    ambient_playing = TRUE;
    
    // Crear hilo si no existe
    if (ambient_thread_handle == NULL) {
        ambient_thread_handle = CreateThread(NULL, 0, ambient_thread, NULL, 0, NULL);
    }
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
    // Detener hilos
    ambient_should_stop = TRUE;
    footstep_should_stop = TRUE;
    running_should_stop = TRUE;
    
    // Esperar a que terminen los hilos
    if (ambient_thread_handle != NULL) {
        WaitForSingleObject(ambient_thread_handle, 1000);
        CloseHandle(ambient_thread_handle);
        ambient_thread_handle = NULL;
    }
    
    if (footstep_thread_handle != NULL) {
        WaitForSingleObject(footstep_thread_handle, 1000);
        CloseHandle(footstep_thread_handle);
        footstep_thread_handle = NULL;
    }
    
    if (running_thread_handle != NULL) {
        WaitForSingleObject(running_thread_handle, 1000);
        CloseHandle(running_thread_handle);
        running_thread_handle = NULL;
    }
    
    // Detener todos los sonidos
    PlaySound(NULL, NULL, 0);
    
    printf("Sistema de audio limpiado\n");
}