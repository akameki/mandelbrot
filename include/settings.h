#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <filesystem>
#include "shader.h"
#include <nlohmann/json.hpp>

struct ChannelState {
    glm::vec3 color = {0.0f, 0.0f, 0.0f};
    // float r, g, b;
    float y_scale = 1.0f;
    float x_offset = 0.0f;
    float x_scale = 0.3f;
    float t_scale = 0.5f;
    bool show_controls = true;
    float operator()(float iteration) {
        return y_scale * (0.5f + 0.5f * glm::cos(x_offset + iteration * x_scale + t_scale * ImGui::GetTime()));
    }
};
struct PaletteState {
    std::unordered_map<std::string, ChannelState> channels;
    bool reversed = false;
    bool override = true;
    bool use_smooth = true;
    glm::vec3 override_color {0.0f, 0.0f, 0.0f};
};
struct AppState {
    int width = 1200, height = 900;
    double camera_x = -0.65, camera_y = 0.0, zoom = 0.5;
    int max_iterations = 150;
    bool auto_zoom_in = false, auto_zoom_out = false;
    bool show_ui = true, use_ssaa = true;
    bool dirty_fractal = true;
    PaletteState palette_state;

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

void load_state(App& app, const std::filesystem::path& path);
void save_state(App& app, const std::filesystem::path& path);

void update_camera(App& app);
void update_uniforms(App& app, ShaderProgram& sp);
void imgui_camera_ui(App& app);

bool is_pressed(GLFWwindow* window, int key);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


/* de/serialization */

// ChannelState
inline void to_json(nlohmann::json& j, const ChannelState& c) {
    j = nlohmann::json{
        {"color", {c.color.x, c.color.y, c.color.z}},
        {"y_scale", c.y_scale},
        {"x_scale", c.x_scale},
        {"x_offset", c.x_offset},
        {"t_scale", c.t_scale},
        {"show_controls", c.show_controls},
    };
}
inline void from_json(const nlohmann::json& j, ChannelState& c) {
    if (j.contains("color")) {
        auto col = j.at("color");
        c.color.x = col[0];
        c.color.y = col[1];
        c.color.z = col[2];
    }
    c.y_scale = j.value("y_scale", c.y_scale);
    c.x_scale = j.value("x_scale", c.x_scale);
    c.x_offset = j.value("x_offset", c.x_offset);
    c.t_scale = j.value("t_scale", c.t_scale);
    c.show_controls = j.value("show_controls", c.show_controls);
}

// PaletteState
inline void to_json(nlohmann::json& j, const PaletteState& p) {
    j = nlohmann::json{
        {"reversed", p.reversed},
        {"override", p.override},
        {"override_color", {p.override_color.x, p.override_color.y, p.override_color.z}},
        {"channels", p.channels},
        {"use_smooth", p.use_smooth}
    };
}
inline void from_json(const nlohmann::json& j, PaletteState& p) {
    p.reversed = j.value("reversed", p.reversed);
    p.override = j.value("override", p.override);
    if (j.contains("override_color")) {
        auto col = j.at("override_color");
        p.override_color.x = col[0];
        p.override_color.y = col[1];
        p.override_color.z = col[2];
    }
    if (j.contains("channels")) {
        p.channels = j.at("channels").get<decltype(p.channels)>();
    }
    p.use_smooth = j.value("use_smooth", p.use_smooth);
}
// AppState
inline void to_json(nlohmann::json& j, const AppState& s) {
    j = nlohmann::json{
        {"width", s.width},
        {"height", s.height},
        {"camera_x", s.camera_x},
        {"camera_y", s.camera_y},
        {"zoom", s.zoom},
        {"max_iterations", s.max_iterations},
        // {"auto_zoom_in", s.auto_zoom_in},
        // {"auto_zoom_out", s.auto_zoom_out},
        {"show_ui", s.show_ui},
        {"use_ssaa", s.use_ssaa},
        // {"dirty_fractal", s.dirty_fractal},
        {"palette_state", s.palette_state},
        // {"pan_speed", s.pan_speed},
        // {"zoom_speed", s.zoom_speed}
    };
}
inline void from_json(const nlohmann::json& j, AppState& s) {
    s.width = j.value("width", s.width);
    s.height = j.value("height", s.height);
    s.camera_x = j.value("camera_x", s.camera_x);
    s.camera_y = j.value("camera_y", s.camera_y);
    s.zoom = j.value("zoom", s.zoom);
    s.max_iterations = j.value("max_iterations", s.max_iterations);
    s.show_ui = j.value("show_ui", s.show_ui);
    s.use_ssaa = j.value("use_ssaa", s.use_ssaa);
    if (j.contains("palette_state")) {
        s.palette_state = j.at("palette_state").get<PaletteState>();
    }
}
