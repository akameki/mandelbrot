#pragma once
#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <vector>
#include <string>

struct Fn {
    std::string label;
    float r, g, b;
    float x_offset = 0.0f;
    float x_scale = 0.3f;
    float y_scale = 1.0f;
    
    Fn(std::string label, float r, float g, float b);
    float operator()(float iteration);
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
