// audio.h - Sistema multi-hilo de audio para Windows
#ifndef AUDIO_H
#define AUDIO_H

#include <windows.h>
#include <mmsystem.h>

// Estructura para sonidos con hilos
typedef struct {
    char* filename;
    BOOL loaded;
    HANDLE thread_handle;
    BOOL is_playing;
    DWORD thread_id;
} Sound;

// Funciones de audio
void init_audio();
void play_footstep_sound();
void play_running_sound();
void play_ambient_sound();
void update_audio();
void cleanup_audio();

// Funciones de hilos
DWORD WINAPI ambient_thread(LPVOID lpParam);
DWORD WINAPI footstep_thread(LPVOID lpParam);
DWORD WINAPI running_thread(LPVOID lpParam);

#endif // AUDIO_H
