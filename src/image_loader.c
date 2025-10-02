// image_loader.c - Sistema simple de carga de imágenes BMP
#include "image_loader.h"
#include <GL/gl.h>
#include <string.h>

ImageData* load_bmp(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error: No se pudo abrir el archivo %s\n", filename);
        return NULL;
    }
    
    // Leer header BMP
    unsigned char header[54];
    if (fread(header, 1, 54, file) != 54) {
        printf("Error: Archivo BMP inválido\n");
        fclose(file);
        return NULL;
    }
    
    // Verificar que es un BMP válido
    if (header[0] != 'B' || header[1] != 'M') {
        printf("Error: No es un archivo BMP válido\n");
        fclose(file);
        return NULL;
    }
    
    // Extraer información de la imagen
    int width = *(int*)&header[18];
    int height = *(int*)&header[22];
    int bits_per_pixel = *(int*)&header[28];
    
    // Solo soportamos 24 bits por pixel
    if (bits_per_pixel != 24) {
        printf("Error: Solo se soportan imágenes de 24 bits por pixel\n");
        fclose(file);
        return NULL;
    }
    
    // Calcular tamaño de datos
    int row_padding = (4 - (width * 3) % 4) % 4;
    int data_size = (width * 3 + row_padding) * height;
    
    // Asignar memoria para la imagen
    ImageData* image = (ImageData*)malloc(sizeof(ImageData));
    if (!image) {
        printf("Error: No se pudo asignar memoria\n");
        fclose(file);
        return NULL;
    }
    
    image->width = width;
    image->height = height;
    image->channels = 3;
    image->data = (unsigned char*)malloc(data_size);
    
    if (!image->data) {
        printf("Error: No se pudo asignar memoria para datos de imagen\n");
        free(image);
        fclose(file);
        return NULL;
    }
    
    // Leer datos de la imagen
    if (fread(image->data, 1, data_size, file) != data_size) {
        printf("Error: No se pudieron leer los datos de la imagen\n");
        free(image->data);
        free(image);
        fclose(file);
        return NULL;
    }
    
    fclose(file);
    
    // Convertir BGR a RGB
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * (width * 3 + row_padding)) + (x * 3);
            unsigned char temp = image->data[index];
            image->data[index] = image->data[index + 2];
            image->data[index + 2] = temp;
        }
    }
    
    printf("Imagen cargada: %dx%d, %d canales\n", width, height, image->channels);
    return image;
}

void free_image(ImageData* image) {
    if (image) {
        if (image->data) {
            free(image->data);
        }
        free(image);
    }
}

GLuint create_texture_from_image(ImageData* image) {
    if (!image || !image->data) {
        return 0;
    }
    
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    // Configurar parámetros de textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    
    // Cargar datos de la imagen
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);
    
    return texture;
}
