#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"

// uniforms
extern int width;
extern int height;
extern double camera_x;
extern double camera_y;
extern double zoom;
extern int iterations;

extern bool show_ui;
extern bool ssaa_toggle;

void update_options(GLFWwindow* window);
void update_uniforms(GLFWwindow* window, ShaderProgram& sp);
bool is_pressed(GLFWwindow* window, int key);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void imgui_camera_ui();
