#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include "imgui.h"
#include "settings.h"
#include "shader.h"

using json = nlohmann::json;

void load_state(App& app, const std::filesystem::path& path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        std::cerr << "Failed to open file for loading state: " << path << std::endl;
        return;
    }
    json data = json::parse(f);
    try {
        app.state = data.get<AppState>();
        app.state.mark_dirty();
    } catch (const json::exception& e) {
        std::cerr << "Error parsing state file: " << e.what() << std::endl;
        return;
    }

    f.close();
}

void save_state(App& app, const std::filesystem::path& path) {
    std::ofstream f(path);
    if (!f.is_open()) {
        std::cerr << "Failed to open file for saving state: " << path << std::endl;
        return;
    }
    json data = app.state;
    f << data.dump(4);
    f.close();
}

void update_camera(App& app) {
    GLFWwindow* window = app.window;
    AppState& state = app.state;
    if (is_pressed(window, GLFW_KEY_W)) state.camera_y += state.pan_speed / state.zoom, state.mark_dirty();
    if (is_pressed(window, GLFW_KEY_A)) state.camera_x -= state.pan_speed / state.zoom, state.mark_dirty();
    if (is_pressed(window, GLFW_KEY_S)) state.camera_y -= state.pan_speed / state.zoom, state.mark_dirty();
    if (is_pressed(window, GLFW_KEY_D)) state.camera_x += state.pan_speed / state.zoom, state.mark_dirty();
    if (state.auto_zoom_out || is_pressed(window, GLFW_KEY_Q)) state.zoom /= state.zoom_speed, state.mark_dirty();
    if (state.auto_zoom_in || is_pressed(window, GLFW_KEY_E)) state.zoom *= state.zoom_speed, state.mark_dirty();

    if (state.is_dragging) {
        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);
        
        double dx = (mouse_x - state.last_mouse_x) * 2.0 / state.width;
        double dy = (mouse_y - state.last_mouse_y) * 2.0 / state.height;

        if (dx != 0.0 || dy != 0.0) {
            double aspect = (double)state.width / (double)state.height;
            state.camera_x -= dx * aspect * 1.0/state.zoom;
            state.camera_y += dy * 1.0/state.zoom;
            
            state.last_mouse_x = mouse_x;
            state.last_mouse_y = mouse_y;
    
            state.mark_dirty();
        }
    }
}

void update_uniforms(App& app, ShaderProgram& sp) {
    GLFWwindow* window = app.window;
    AppState& state = app.state;
    glUniform1f(sp.uniform_location("time"), glfwGetTime());
    glUniform2d(sp.uniform_location("camera"), state.camera_x, state.camera_y);
    glUniform1d(sp.uniform_location("zoom"), state.zoom);
    glUniform2f(sp.uniform_location("resolution"), state.width, state.height);
    glUniform1i(sp.uniform_location("iterations"), state.max_iterations);
}

bool is_pressed(GLFWwindow* window, int key) {
    return glfwGetKey(window, key) != GLFW_RELEASE;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    AppState& state = app->state;
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) state.show_ui = !state.show_ui;
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        state.pan_speed = 0.02;
        state.zoom_speed = 1.01;
    } else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
        state.pan_speed = 0.01;
        state.zoom_speed = 1.005;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (ImGui::GetIO().WantCaptureMouse) return;
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    AppState& state = app->state;
    
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            state.is_dragging = true;
            glfwGetCursorPos(window, &state.last_mouse_x, &state.last_mouse_y);
        } else if (action == GLFW_RELEASE) {
            state.is_dragging = false;
        }
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (ImGui::GetIO().WantCaptureMouse) return;
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    AppState& state = app->state;

    double zoom_factor = (yoffset < 0) ? 0.9 : 1.1;
    
    double mouse_x, mouse_y;
    glfwGetCursorPos(window, &mouse_x, &mouse_y);

    double aspect = (double)state.width / (double)state.height;
    double fractal_mouse_x = ((mouse_x / state.width) * 2.0 - 1.0) * aspect;
    double fractal_mouse_y = 1.0 - (mouse_y / state.height) * 2.0;

    double fractal_target_x = state.camera_x + fractal_mouse_x / state.zoom;
    double fractal_target_y = state.camera_y + fractal_mouse_y / state.zoom;

    state.zoom *= zoom_factor;

    state.camera_x = fractal_target_x - fractal_mouse_x / state.zoom;
    state.camera_y = fractal_target_y - fractal_mouse_y / state.zoom;
    state.mark_dirty();
}

void imgui_camera_ui(App& app) {
    AppState& state = app.state;
    ImGui::SeparatorText("Camera");

    // ImGui::PushItemWidth(160); // with arrows
    ImGui::PushItemWidth(120);
    ImGui::AlignTextToFramePadding(); ImGui::Text("x=   "); ImGui::SameLine();
    if (ImGui::InputDouble("##cam_x", &state.camera_x, 0.0, 0.0, "%15.12f")) state.mark_dirty();
    ImGui::AlignTextToFramePadding(); ImGui::Text("y=   "); ImGui::SameLine();
    if (ImGui::InputDouble("##cam_y", &state.camera_y, 0.0, 0.0, "%15.12f")) state.mark_dirty();
    ImGui::AlignTextToFramePadding(); ImGui::Text("zoom="); ImGui::SameLine();
    if (ImGui::InputDouble("##zoom", &state.zoom, 0.0, 0.0, "%15.12f")) state.mark_dirty();

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Auto zoom"); ImGui::SameLine();
    if (ImGui::Checkbox("In", &state.auto_zoom_in)) state.auto_zoom_out = false;
    ImGui::SameLine();
    if (ImGui::Checkbox("Out", &state.auto_zoom_out)) state.auto_zoom_in = false;

    if (ImGui::Button("Reset")) {
        state.camera_x = -0.65f;
        state.camera_y = 0.0f;
        state.zoom = 0.5f;
        state.auto_zoom_in = state.auto_zoom_out = false;
        state.mark_dirty();
    }
}
