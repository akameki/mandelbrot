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

void update_options(GLFWwindow* window, ShaderProgram& sp);
bool is_pressed(GLFWwindow* window, int key);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void imgui_camera_ui();
