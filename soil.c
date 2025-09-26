// SOIL.c - Simple OpenGL Image Library Implementation
#include "soil.h"
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Simple BMP loader for SOIL
static unsigned char* load_bmp(const char* filename, int* width, int* height, int* channels) {
    FILE* file = fopen(filename, "rb");
    if (!file) return NULL;
    
    // Read BMP header
    unsigned char header[54];
    if (fread(header, 1, 54, file) != 54) {
        fclose(file);
        return NULL;
    }
    
    // Check BMP signature
    if (header[0] != 'B' || header[1] != 'M') {
        fclose(file);
        return NULL;
    }
    
    // Get dimensions
    *width = *(int*)&header[18];
    *height = *(int*)&header[22];
    *channels = 3; // RGB
    
    // Allocate memory
    int data_size = *width * *height * 3;
    unsigned char* data = (unsigned char*)malloc(data_size);
    if (!data) {
        fclose(file);
        return NULL;
    }
    
    // Read image data (BMP is upside down)
    for (int i = *height - 1; i >= 0; i--) {
        fread(data + i * *width * 3, 1, *width * 3, file);
    }
    
    fclose(file);
    return data;
}

// SOIL_load_OGL_texture implementation
unsigned int SOIL_load_OGL_texture(const char *filename, int force_channels, unsigned int reuse_texture_ID, unsigned int flags) {
    int width, height, channels;
    unsigned char* image_data = load_bmp(filename, &width, &height, &channels);
    
    if (!image_data) {
        printf("SOIL: Error loading image %s\n", filename);
        return 0;
    }
    
    // Generate texture
    unsigned int texture_id;
    if (reuse_texture_ID == 0) {
        glGenTextures(1, &texture_id);
    } else {
        texture_id = reuse_texture_ID;
    }
    
    glBindTexture(GL_TEXTURE_2D, texture_id);
    
    // Configure texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // Convert BGR to RGB
    for (int i = 0; i < width * height; i++) {
        int idx = i * 3;
        unsigned char temp = image_data[idx];
        image_data[idx] = image_data[idx + 2];
        image_data[idx + 2] = temp;
    }
    
    // Load texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    
    free(image_data);
    
    printf("SOIL: Loaded texture %s (%dx%d)\n", filename, width, height);
    return texture_id;
}

// Other SOIL functions (stubs)
unsigned int SOIL_load_OGL_texture_from_memory(const unsigned char *const buffer, int buffer_length, int force_channels, unsigned int reuse_texture_ID, unsigned int flags) {
    return 0;
}

unsigned int SOIL_load_OGL_cubemap(const char *x_pos_file, const char *x_neg_file, const char *y_pos_file, const char *y_neg_file, const char *z_pos_file, const char *z_neg_file, int force_channels, unsigned int reuse_texture_ID, unsigned int flags) {
    return 0;
}

unsigned int SOIL_load_OGL_single_cubemap(const char *filename, const char face_order[6], int force_channels, unsigned int reuse_texture_ID, unsigned int flags) {
    return 0;
}

unsigned int SOIL_load_OGL_HDR_texture(const char *filename, int fake_HDR_format, int rescale_to_max, unsigned int reuse_texture_ID, unsigned int flags) {
    return 0;
}

unsigned int SOIL_load_OGL_texture_type(const char *filename, int force_channels, unsigned int reuse_texture_ID, unsigned int flags, int type) {
    return 0;
}

unsigned int SOIL_create_OGL_texture(const unsigned char *const data, int width, int height, int channels, unsigned int reuse_texture_ID, unsigned int flags) {
    return 0;
}

unsigned int SOIL_create_OGL_single_cubemap(const unsigned char *const data, int width, int height, int channels, unsigned int reuse_texture_ID, unsigned int flags) {
    return 0;
}

int SOIL_save_screenshot(const char *filename, int image_type, int x, int y, int width, int height) {
    return 0;
}

unsigned char* SOIL_load_image(const char *filename, int *width, int *height, int *channels, int force_channels) {
    return load_bmp(filename, width, height, channels);
}

void SOIL_free_image_data(unsigned char *img_data) {
    free(img_data);
}

const char* SOIL_last_result(void) {
    return "SOIL: No error";
}
