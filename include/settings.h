#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"



// struct PaletteSettings {

// };
struct AppState {
    int width = 1200, height = 900;
    double camera_x = 0.0, camera_y = 0.0, zoom = 0.5;
    int max_iterations = 150;
    bool auto_zoom_in = false, auto_zoom_out = false;
    bool show_ui = true, use_ssaa = true, use_smooth_colors = true;
    bool dirty_fractal = true;
    // PaletteSettings palette_settings;

    // camera
    double pan_speed = 0.01;
    double zoom_speed = 1.005;

    bool is_dragging = false;
    double last_mouse_x = 0.0;
    double last_mouse_y = 0.0;
    double mouse_pan_speed = 0.002; // Adjust this to control panning sensitivity

    void mark_dirty() { dirty_fractal = true; }
};

struct App {
    GLFWwindow* window = nullptr;
    AppState state;
    // AppResources resources;
};
// struct AppResources {

// };

void update_camera(App& app);
void update_uniforms(App& app, ShaderProgram& sp);
void imgui_camera_ui(App& app);

bool is_pressed(GLFWwindow* window, int key);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
