// events.h - Eventos al entrar en zonas
#ifndef EVENTS_H
#define EVENTS_H

// Tipos de eventos
typedef enum {
    EVENT_NONE,
    EVENT_SCREEN_CHANGE,
    EVENT_AMBIENT_SOUND,
    EVENT_LIGHTING_CHANGE,
    EVENT_OBJECT_INTERACTION
} EventType;

// Funciones de eventos
void init_events();
void trigger_event(EventType event_type, void* data);
void process_events();
void cleanup_events();

#endif // EVENTS_H
