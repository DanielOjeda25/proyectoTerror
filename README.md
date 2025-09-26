# Proyecto Deeper - Estructura Modular

## Estructura del Proyecto

```
proyectoTerror/
├── include/              # Headers de librerías
│   ├── GL/              # Headers de OpenGL
│   │   ├── glew.h
│   │   ├── gl.h
│   │   └── ...
│   ├── GLFW/            # Headers de GLFW
│   │   ├── glfw3.h
│   │   └── ...
│   ├── stb_easy_font.h  # Librería de fuentes
│   └── stb_image.h      # Cargador de imágenes
├── src/                 # Código fuente
│   ├── main.c          # Inicialización, loop principal
│   ├── input.c/h       # Manejo de teclado y mouse
│   ├── render.c/h      # Funciones de dibujo
│   ├── map.c/h         # Definición del mapa y triggers
│   ├── events.c/h      # Eventos al entrar en zonas
│   └── player.c/h      # Posición, movimiento, colisiones
├── assets/             # Recursos del juego
│   ├── image.bmp       # Texturas
│   └── models/         # Modelos 3D (opcional)
├── lib/                # Librerías compiladas
│   ├── glfw3.lib
│   ├── glew32.lib
│   └── opengl32.lib
└── Makefile           # Script de compilación
```

## Compilación

```bash
# Compilar todo el proyecto
make

# Compilar solo un módulo
make input
make render

# Limpiar archivos compilados
make clean
```

## Módulos

- **main.c**: Loop principal y inicialización
- **input.c/h**: Manejo de entrada (teclado, mouse)
- **render.c/h**: Sistema de renderizado
- **map.c/h**: Sistema de mapas y triggers
- **events.c/h**: Sistema de eventos
- **player.c/h**: Lógica del jugador

## Próximos Pasos

1. Mover código existente a los módulos correspondientes
2. Implementar funcionalidades específicas en cada módulo
3. Agregar más assets (texturas, modelos)
4. Expandir sistema de eventos
