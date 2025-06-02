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
double zoom = 1.0;
int iterations = 125;

// other globals
bool show_ui = true;

bool auto_zoom_in = false;
bool auto_zoom_out = false;
double pan_speed = 0.01;
double zoom_speed = 1.005;

void update_options(GLFWwindow* window, ShaderProgram& sp) {
    if (is_pressed(window, GLFW_KEY_W)) camera_y += pan_speed * zoom;
    if (is_pressed(window, GLFW_KEY_A)) camera_x -= pan_speed * zoom;
    if (is_pressed(window, GLFW_KEY_S)) camera_y -= pan_speed * zoom;
    if (is_pressed(window, GLFW_KEY_D)) camera_x += pan_speed * zoom;
    if (auto_zoom_out || is_pressed(window, GLFW_KEY_Q)) zoom *= zoom_speed;
    if (auto_zoom_in || is_pressed(window, GLFW_KEY_E)) zoom /= zoom_speed;
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
        zoom = 1.0;
        auto_zoom_in = auto_zoom_out = false;
    }
}
