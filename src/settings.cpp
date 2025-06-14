#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "settings.h"
#include "shader.h"

// uniforms
int width = 1200;
int height = 900;
double camera_x = 0.0;
double camera_y = 0.0;
double zoom = 0.5;
int iterations = 125;

// other globals
bool show_ui = true;

bool auto_zoom_in = false;
bool auto_zoom_out = false;
double pan_speed = 0.01;
double zoom_speed = 1.005;


// Add these global variables with your other globals
bool is_dragging = false;
double last_mouse_x = 0.0;
double last_mouse_y = 0.0;
double mouse_pan_speed = 0.002; // Adjust this to control panning sensitivity

void update_options(GLFWwindow* window) {
    if (is_pressed(window, GLFW_KEY_W)) camera_y += pan_speed * zoom;
    if (is_pressed(window, GLFW_KEY_A)) camera_x -= pan_speed * zoom;
    if (is_pressed(window, GLFW_KEY_S)) camera_y -= pan_speed * zoom;
    if (is_pressed(window, GLFW_KEY_D)) camera_x += pan_speed * zoom;
    if (auto_zoom_out || is_pressed(window, GLFW_KEY_Q)) zoom /= zoom_speed;
    if (auto_zoom_in || is_pressed(window, GLFW_KEY_E)) zoom *= zoom_speed;

    if (is_dragging) {
        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);
        
        double dx = (mouse_x - last_mouse_x) * 2.0 / width;
        double dy = (mouse_y - last_mouse_y) * 2.0 / height;
        
        double aspect = (double)width / (double)height;
        camera_x -= dx * aspect * 1.0/zoom;
        camera_y += dy * 1.0/zoom;
        
        last_mouse_x = mouse_x;
        last_mouse_y = mouse_y;
    }
}

void update_uniforms(GLFWwindow* window, ShaderProgram& sp) {
    glUniform1f(sp.uniform_location("time"), glfwGetTime());
    glUniform2d(sp.uniform_location("camera"), camera_x, camera_y);
    glUniform1d(sp.uniform_location("zoom"), zoom);
    glUniform2f(sp.uniform_location("resolution"), width, height);
    glUniform1i(sp.uniform_location("iterations"), iterations);
}

bool is_pressed(GLFWwindow* window, int key) {
    return glfwGetKey(window, key) != GLFW_RELEASE;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) show_ui = !show_ui;
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        pan_speed = 0.02;
        zoom_speed = 1.01;
    } else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
        pan_speed = 0.01;
        zoom_speed = 1.005;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return; // Don't handle if ImGui is using the mouse
    
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            is_dragging = true;
            glfwGetCursorPos(window, &last_mouse_x, &last_mouse_y);
        } else if (action == GLFW_RELEASE) {
            is_dragging = false;
        }
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return; // Don't handle if ImGui is using the mouse
    
    double zoom_factor = (yoffset < 0) ? 0.9 : 1.1;
    
    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);

    double aspect = (double)width / (double)height;
    double fractal_mouse_x = ((mouse_x / width) * 2.0 - 1.0) * aspect;
    double fractal_mouse_y = 1.0 - (mouse_y / height) * 2.0;

    double fractal_target_x = camera_x + fractal_mouse_x / zoom;
    double fractal_target_y = camera_y + fractal_mouse_y / zoom;

    zoom *= zoom_factor;

    camera_x = fractal_target_x - fractal_mouse_x / zoom;
    camera_y = fractal_target_y - fractal_mouse_y / zoom;
}

void imgui_camera_ui() {
    ImGui::SeparatorText("Camera");
    // const double d_cam = 3.0 * pan_speed * zoom;
    // const double d_zoom = 3.0 * zoom * (zoom_speed-1.0);

    // ImGui::PushItemWidth(160); // with arrows
    ImGui::PushItemWidth(120);
    ImGui::AlignTextToFramePadding(); ImGui::Text("x=   "); ImGui::SameLine();
    ImGui::InputDouble("##cam_x", &camera_x, 0.0, 0.0, "%15.12f");
    ImGui::AlignTextToFramePadding(); ImGui::Text("y=   "); ImGui::SameLine();
    ImGui::InputDouble("##cam_y", &camera_y, 0.0, 0.0, "%15.12f");
    ImGui::AlignTextToFramePadding(); ImGui::Text("zoom="); ImGui::SameLine();
    ImGui::InputDouble("##zoom", &zoom, 0.0, 0.0, "%15.12f");

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Auto zoom"); ImGui::SameLine();
    if (ImGui::Checkbox("In", &auto_zoom_in)) auto_zoom_out = false;
    ImGui::SameLine();
    if (ImGui::Checkbox("Out", &auto_zoom_out)) auto_zoom_in = false;

    if (ImGui::Button("Reset")) {
        camera_x = camera_y = 0.0;
        zoom = 0.5;
        auto_zoom_in = auto_zoom_out = false;
    }
}
