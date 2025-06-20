#pragma once
#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <vector>
#include "settings.h"
#include <string>

// `0.5f + 0.5f * cos(x_offset + iteration * x_scale + time_scale*secs);`
// float operator()(float iteration);

// Manages a 1D texture of RGB colors, and the UI to control it.
class Palette {
private:
    PaletteState* state;
    GLuint texture;
    std::vector<glm::vec3> colors;
public:

    Palette(PaletteState* state);
    // bool add_channel(std::string& label);
    void draw_channel_settings(const std::string& name, ChannelState& channel);

    void reverse();
    // void resize(int size);
    void generate(int num_colors);
    void update_filter();
    void bind_texture();
    void unbind_texture();
    void draw_ui();

};
