// input.h - Sistema de input para Backrooms 3D
#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>
#include <stdbool.h>

// Variables globales de input
extern bool keys[GLFW_KEY_LAST + 1];
extern bool mouseCaptured;
extern double lastMouseX, lastMouseY;

// Funciones de input
void init_input();
void setup_input_callbacks(GLFWwindow* window);
void process_input(GLFWwindow* window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
bool is_key_pressed(int key);
void cleanup_input();

#endif // INPUT_H
