# Makefile para proyecto Deeper - Sistema 3D
CC = gcc
CFLAGS = -Iinclude -Wall -O2 -std=c99
LDFLAGS = -Llib -lglfw3 -lopengl32 -lglu32 -lgdi32 -lwinmm

# Archivos fuente (sistema modular)
SOURCES = src/main.c src/player.c src/input.c src/render.c src/map.c src/events.c src/image_loader.c src/audio.c src/particles.c
TARGET = PROYECTOTERROR.exe

# Regla principal
all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) $(LDFLAGS) -o $(TARGET)

# Limpiar archivos compilados
clean:
	del $(TARGET)

# Compilar solo un módulo (para testing)
input: src/input.c
	$(CC) $(CFLAGS) -c src/input.c -o src/input.o

render: src/render.c
	$(CC) $(CFLAGS) -c src/render.c -o src/render.o

.PHONY: all clean input render
