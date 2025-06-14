#pragma once
#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <vector>
#include <string>

    // `0.5f + 0.5f * cos(x_offset + iteration * x_scale + time_scale*secs);`
struct Fn {
    std::string label;
    float r, g, b;
    float x_offset = 0.0f;
    float x_scale = 0.3f;
    float t_scale = 1.0f;
    bool show_controls = true;
    
    Fn(std::string label, float r, float g, float b);
    float operator()(float iteration);
    void draw_ui();
};

// Manages a 1D texture of RGB colors, and the UI to control it.
class Palette {
private:
    // look into overhead with using texture vs array

    GLuint texture;
    int num_colors;
    std::vector<Fn> channels;
    std::vector<glm::vec3> colors;
    bool reversed = false;
    bool override = true;
    glm::vec3 override_color {0.0f, 0.0f, 0.0f};

public:

    Palette(int size = 100);

    void reverse();
    void resize(int size);
    void update();
    void bind();
    void unbind();
    void draw_ui();

};
