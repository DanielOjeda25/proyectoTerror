#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "stb_easy_font.h"
#include "soil.h"  

bool enPantalla2 = false;
int windowWidth = 1920;
int windowHeight = 1080;
GLuint backgroundTexture = 0;
int imageWidth, imageHeight, imageChannels;

// Cargar textura de fondo usando SOIL
bool cargarTexturaFondo(const char* filename) {
    backgroundTexture = SOIL_load_OGL_texture(filename, SOIL_LOAD_RGB, 0, 0);
    
    if (backgroundTexture == 0) {
        printf("SOIL Error: %s\n", SOIL_last_result());
        return false;
    }
    
    printf("SOIL: Imagen cargada exitosamente\n");
    return true;
}

// Dibujar fondo con textura ocupando toda la pantalla
void dibujarFondo() {
    if (backgroundTexture == 0) return;
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    
    // Usar coordenadas de textura estándar (0,0) a (1,1)
    glColor3f(1.0f, 1.0f, 1.0f); // Color blanco para no afectar la textura
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f); // Esquina inferior izquierda
        glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f, -1.0f); // Esquina inferior derecha
        glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f,  1.0f); // Esquina superior derecha
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f,  1.0f); // Esquina superior izquierda
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
}

// Dibujar texto con stb_easy_font
void dibujarTexto(const char* texto, float x, float y, float r, float g, float b) {
    char buffer[99999]; // suficiente para mucho texto
    int quads;

    quads = stb_easy_font_print(x, y, (char*)texto, NULL, buffer, sizeof(buffer));

    // Escalar el texto para hacerlo más visible
    glPushMatrix();
    glScalef(0.01f, 0.01f, 1.0f); // Escalar a 1/100 del tamaño original (más pequeño)
    glScalef(1.0f, -1.0f, 1.0f); // Invertir Y para corregir la orientación
    
    glColor3f(r, g, b);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 16, buffer);
    glDrawArrays(GL_QUADS, 0, quads * 4);
    glDisableClientState(GL_VERTEX_ARRAY);
    
    glPopMatrix();
}

// Botón central
void dibujarBoton() {
    // Rectángulo azul centrado
    glColor3f(0.2f, 0.4f, 0.8f);
    glBegin(GL_QUADS);
        glVertex2f(-0.15f, -0.08f);
        glVertex2f( 0.15f, -0.08f);
        glVertex2f( 0.15f,  0.08f);
        glVertex2f(-0.15f,  0.08f);
    glEnd();

    // Texto "Iniciar" centrado en el botón
    dibujarTexto("Iniciar", -1.2f, 0.0f, 1.0f, 1.0f, 1.0f);
}

void dibujarPantalla2() {

    // Texto "Pantalla 2"
    dibujarTexto("Pantalla 2", -6.0f, 0.0f, 1.0f, 1.0f, 1.0f); // blanco
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !enPantalla2) {
        double xpos, ypos;
        int width, height;
        glfwGetCursorPos(window, &xpos, &ypos);
        glfwGetWindowSize(window, &width, &height);

        float nx = (xpos / (float)width) * 2.0f - 1.0f;
        float ny = 1.0f - (ypos / (float)height) * 2.0f;

        float bx1 = -0.15f, by1 = -0.08f;
        float bx2 =  0.15f, by2 =  0.08f;

        if (nx >= bx1 && nx <= bx2 && ny >= by1 && ny <= by2) {
            enPantalla2 = true;
        }
    }
}

void window_size_callback(GLFWwindow* window, int width, int height) {
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
    
    // Reconfigurar la matriz de proyección para mantener aspecto correcto
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Calcular aspect ratio correcto
    float aspect = (float)width / (float)height;
    if (aspect > 1.0f) {
        // Ventana más ancha que alta
        glOrtho(-aspect, aspect, -1.0, 1.0, -1.0, 1.0);
    } else {
        // Ventana más alta que ancha
        glOrtho(-1.0, 1.0, -1.0/aspect, 1.0/aspect, -1.0, 1.0);
    }
    
    glMatrixMode(GL_MODELVIEW);
}

int main() {
    if (!glfwInit()) {
        printf("Error al inicializar GLFW\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Deeper", NULL, NULL);
    glfwSetWindowSizeLimits(window, 1280, 720, GLFW_DONT_CARE, GLFW_DONT_CARE);
    if (!window) {
        printf("No se pudo crear la ventana\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    
    // Configurar la matriz de proyección para mantener aspecto correcto
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Calcular aspect ratio para 1920x1080
    float aspect = 1920.0f / 1080.0f;
    glOrtho(-aspect, aspect, -1.0, 1.0, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Cargar imagen de fondo
    if (!cargarTexturaFondo("image.bmp")) {
        printf("Advertencia: No se pudo cargar image.bmp, intentando image.jpg...\n");
        if (!cargarTexturaFondo("image.jpg")) {
            printf("Advertencia: No se pudo cargar ninguna imagen, usando fondo blanco\n");
            printf("Para usar tu imagen de backrooms:\n");
            printf("1. Instala Python y Pillow: pip install Pillow\n");
            printf("2. Ejecuta: python convert_image.py\n");
            printf("3. Esto convertirá image.jpg a image.bmp\n");
        } else {
            printf("¡Imagen JPEG cargada exitosamente!\n");
        }
    } else {
        printf("¡Imagen BMP cargada exitosamente!\n");
    }

    while (!glfwWindowShouldClose(window)) {
        if (!enPantalla2) {
            // Fondo blanco (fallback si no hay imagen)
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        } else {
            // Fondo negro
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        }
    
        // Limpiar SIEMPRE antes de dibujar
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Resetear la matriz de modelo para cada frame
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
    
        if (!enPantalla2) {
            // Dibujar imagen de fondo si está disponible
            if (backgroundTexture != 0) {
                dibujarFondo();
            }
            dibujarBoton();
        } else {
            dibujarPantalla2();
        }
    
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
