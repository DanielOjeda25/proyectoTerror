// SOIL.h - Simple OpenGL Image Library
#ifndef SOIL_H
#define SOIL_H

#ifdef __cplusplus
extern "C" {
#endif

// SOIL constants
#define SOIL_LOAD_AUTO 0
#define SOIL_LOAD_L 1
#define SOIL_LOAD_LA 2
#define SOIL_LOAD_RGB 3
#define SOIL_LOAD_RGBA 4

// SOIL flags
#define SOIL_FLAG_POWER_OF_TWO 1
#define SOIL_FLAG_MIPMAPS 2
#define SOIL_FLAG_TEXTURE_REPEATS 4
#define SOIL_FLAG_MULTIPLY_ALPHA 8
#define SOIL_FLAG_INVERT_Y 16
#define SOIL_FLAG_COMPRESS_TO_DXT 32
#define SOIL_FLAG_DDS_LOAD_DIRECT 64
#define SOIL_FLAG_NTSC_SAFE_RGB 128
#define SOIL_FLAG_CoCg_Y 256
#define SOIL_FLAG_TEXTURE_RECTANGLE 512

// SOIL functions
unsigned int SOIL_load_OGL_texture(const char *filename, int force_channels, unsigned int reuse_texture_ID, unsigned int flags);
unsigned int SOIL_load_OGL_texture_from_memory(const unsigned char *const buffer, int buffer_length, int force_channels, unsigned int reuse_texture_ID, unsigned int flags);
unsigned int SOIL_load_OGL_cubemap(const char *x_pos_file, const char *x_neg_file, const char *y_pos_file, const char *y_neg_file, const char *z_pos_file, const char *z_neg_file, int force_channels, unsigned int reuse_texture_ID, unsigned int flags);
unsigned int SOIL_load_OGL_single_cubemap(const char *filename, const char face_order[6], int force_channels, unsigned int reuse_texture_ID, unsigned int flags);
unsigned int SOIL_load_OGL_HDR_texture(const char *filename, int fake_HDR_format, int rescale_to_max, unsigned int reuse_texture_ID, unsigned int flags);
unsigned int SOIL_load_OGL_texture_type(const char *filename, int force_channels, unsigned int reuse_texture_ID, unsigned int flags, int type);
unsigned int SOIL_create_OGL_texture(const unsigned char *const data, int width, int height, int channels, unsigned int reuse_texture_ID, unsigned int flags);
unsigned int SOIL_create_OGL_single_cubemap(const unsigned char *const data, int width, int height, int channels, unsigned int reuse_texture_ID, unsigned int flags);

// SOIL utility functions
int SOIL_save_screenshot(const char *filename, int image_type, int x, int y, int width, int height);
unsigned char* SOIL_load_image(const char *filename, int *width, int *height, int *channels, int force_channels);
void SOIL_free_image_data(unsigned char *img_data);
const char* SOIL_last_result(void);

#ifdef __cplusplus
}
#endif

#endif // SOIL_H
