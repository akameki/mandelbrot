#include <vector>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>


#include "palette.h"

Palette::Palette(int size) : size(size) {
    glGenTextures(1, &texture);
    bind();

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); // to catch bugs
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float border_color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_1D, GL_TEXTURE_BORDER_COLOR, border_color);

    // Set the palette colors
    resize(size);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, size, 0, GL_RGB, GL_FLOAT, colors.data());
    unbind();
}

void Palette::resize(int size) {
    this->size = size;
    colors.resize(size);
    for (int i = 0; i < size; ++i) {
        const float pct = (float)i / size;
        glm::vec3 period = 3.0f * glm::vec3{1.3f, 1.1f, 0.9f};
        colors[i] = glm::vec3(0.5f - (glm::cos(pct * period)) / 2.0f);
    }
    bind();
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, size, 0, GL_RGB, GL_FLOAT, colors.data());
    unbind();
}

void Palette::bind() { glBindTexture(GL_TEXTURE_1D, texture); }
void Palette::unbind() { glBindTexture(GL_TEXTURE_1D, 0); }

void Palette::draw_ui() {
    ImGui::Text("Color Palette");
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    float height = ImGui::GetFrameHeight();
    ImVec2 rect_size(ImGui::CalcItemWidth() - height - 2, height); // TODO: fix sizing
    ImVec2 p0 = ImGui::GetCursorScreenPos(); // top left
    ImVec2 p1 = ImVec2(p0.x + rect_size.x, p0.y + rect_size.y); // bottom right
    
    // Draw the current palette
    for (int i = 0; i < size - 1; i++) {
        glm::vec3 col = colors[i];
        ImVec2 rect_p0(p0.x + (rect_size.x * i/(size-1)), p0.y);
        ImVec2 rect_p1(p0.x + (rect_size.x * (i+1)/(size-1)), p1.y);
        draw_list->AddRectFilled(rect_p0, rect_p1, IM_COL32(col.r*255, col.g*255, col.b*255, 255));
    }
    ImGui::InvisibleButton("##palette0", rect_size);
    ImGui::SameLine();
    p0 = ImGui::GetCursorScreenPos();
    draw_list->AddRectFilled({p0.x, p0.y}, {p0.x + 2, p1.y}, IM_COL32(0,0,0,0));
    glm::vec3 col = colors.back(); // last color (color of mandelbrot set; bailout not reached)
    draw_list->AddRectFilled({p0.x + 2, p0.y}, {p0.x + 2 + height, p1.y}, IM_COL32(col.r*255, col.g*255, col.b*255, 255));
    ImGui::InvisibleButton("##idk", {height+2, height});

    // TODO: tooltip
}
