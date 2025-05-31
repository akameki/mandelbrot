#pragma once
#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <vector>
#include "imgui.h"

class Palette {
private:
    // look into overhead with using texture vs array
    GLuint texture;
    int size;
    std::vector<glm::vec3> colors;

public:

    Palette(int size);

    void resize(int size);
    void bind();
    void unbind();
    void draw_ui();

};
