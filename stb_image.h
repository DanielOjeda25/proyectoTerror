// stb_image.h - v2.27 - public domain image loader
#ifndef STB_IMAGE_INCLUDE_STB_IMAGE_H
#define STB_IMAGE_INCLUDE_STB_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef STBI_NO_STDIO
#include <stdio.h>
#endif

typedef unsigned char stbi_uc;

#ifdef __cplusplus
}
#endif

// Implementation
#ifdef STB_IMAGE_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// Simple BMP loader implementation
stbi_uc *stbi_load(char const *filename, int *x, int *y, int *channels_in_file, int desired_channels)
{
    printf("Intentando cargar: %s\n", filename);
    
    FILE *f = fopen(filename, "rb");
    if (!f) {
        printf("Error: No se pudo abrir el archivo %s\n", filename);
        return NULL;
    }
    
    // Leer header BMP
    unsigned char header[54];
    if (fread(header, 1, 54, f) != 54) {
        printf("Error: Archivo inválido\n");
        fclose(f);
        return NULL;
    }
    
    // Verificar que es un BMP válido
    if (header[0] != 'B' || header[1] != 'M') {
        printf("Error: No es un archivo BMP válido\n");
        fclose(f);
        return NULL;
    }
    
    // Obtener dimensiones
    *x = *(int*)&header[18];
    *y = *(int*)&header[22];
    *channels_in_file = 3; // Asumimos RGB
    
    printf("Imagen BMP cargada: %dx%d\n", *x, *y);
    
    // Leer datos de imagen
    int data_size = *x * *y * 3;
    stbi_uc *data = (stbi_uc*)malloc(data_size);
    if (!data) {
        printf("Error: No se pudo asignar memoria\n");
        fclose(f);
        return NULL;
    }
    
    // Leer datos de imagen (BMP está al revés)
    for (int i = *y - 1; i >= 0; i--) {
        fread(data + i * *x * 3, 1, *x * 3, f);
    }
    
    fclose(f);
    return data;
}

void stbi_image_free(void *retval_from_stbi_load)
{
    free(retval_from_stbi_load);
}

#ifdef __cplusplus
}
#endif

#endif // STB_IMAGE_IMPLEMENTATION

#endif // STB_IMAGE_INCLUDE_STB_IMAGE_H
