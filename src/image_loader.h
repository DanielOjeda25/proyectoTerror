// image_loader.h - Sistema simple de carga de imágenes BMP
#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>

// Estructura para datos de imagen
typedef struct {
    unsigned char* data;
    int width;
    int height;
    int channels;
} ImageData;

// Funciones de carga de imágenes
ImageData* load_bmp(const char* filename);
void free_image(ImageData* image);
GLuint create_texture_from_image(ImageData* image);

#endif // IMAGE_LOADER_H
