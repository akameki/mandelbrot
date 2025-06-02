#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include "imgui.h"
#include "implot.h"

#include <vector>
#include <string>

#include "palette.h"

Palette::Palette(int size) : num_colors(size) , reverse(false) {
    glGenTextures(1, &texture);
    bind();

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); // to catch bugs
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float border_color[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_1D, GL_TEXTURE_BORDER_COLOR, border_color);

    // create color channels
    channels = std::vector<Fn>{Fn("r", 1,0,0), Fn("g", 0,1,0), Fn("b", 0,0,1)};
    channels[0].x_offset = 3.14f;
    channels[1].x_offset = 3.87f;
    channels[2].x_offset = 0.79f;
    channels[0].x_scale = 0.3217f;
    channels[1].x_scale = 0.1233f;

    // Set the palette colors
    update();
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, size, 0, GL_RGB, GL_FLOAT, colors.data());
    unbind();
}

void Palette::resize(int size) {
    this->num_colors = size;
    update();
}

void Palette::update() {
    colors.resize(num_colors);
    for (int i = 0; i < num_colors; ++i) {
        // const float pct = (float)i / size;
        // const float pct = i * 0.03;
        // glm::vec3 period = 10.0f * glm::vec3{1.3f, 1.1f, 0.9f};
        // colors[reverse ? num_colors - i - 1 : i] = glm::vec3(0.5f - (glm::cos(2.0f + pct * period)) / 2.0f);
        colors[reverse ? num_colors - i - 1 : i] = {channels[0](i), channels[1](i), channels[2](i)};
    }
    if (override) colors.back() = override_color;
    bind();
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, num_colors, 0, GL_RGB, GL_FLOAT, colors.data());
    unbind();
}

// TODO: linear/smooth coloring in shader (will also need to set filtering to linear instead of nearest)

void Palette::bind() { glBindTexture(GL_TEXTURE_1D, texture); }
void Palette::unbind() { glBindTexture(GL_TEXTURE_1D, 0); }

void Palette::draw_ui() {
    ImGui::Text("Color Palette");
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    float height = ImGui::GetFrameHeight();
    ImVec2 rect_size(ImGui::CalcItemWidth(), height); 
    ImVec2 p0 = ImGui::GetCursorScreenPos(); // top left
    ImVec2 p1 = ImVec2(p0.x + rect_size.x, p0.y + rect_size.y); // bottom right
    
    // Draw the current palette
    for (int i = 0; i < num_colors - 1; i++) {
        glm::vec3 col = colors[i];
        ImVec2 rect_p0(p0.x + (rect_size.x * i/(num_colors-1)), p0.y);
        ImVec2 rect_p1(p0.x + (rect_size.x * (i+1)/(num_colors-1)), p1.y);
        draw_list->AddRectFilled(rect_p0, rect_p1, IM_COL32(col.r*255, col.g*255, col.b*255, 255));
    }

    // TODO: fix sizing
    
    ImGui::InvisibleButton("##palette0", rect_size);
    ImGui::SameLine();
    p0 = ImGui::GetCursorScreenPos();
    draw_list->AddRectFilled({p0.x, p0.y}, {p0.x + 2, p1.y}, IM_COL32(0,0,0,0));
    glm::vec3 col = colors.back(); // last color (color of mandelbrot set; bailout not reached)
    draw_list->AddRectFilled({p0.x + 2, p0.y}, {p0.x + 2 + height, p1.y}, IM_COL32(col.r*255, col.g*255, col.b*255, 255));
    ImGui::InvisibleButton("##idk", {height+2, height});
    if (ImGui::Button("Invert")) {
        reverse = !reverse;
        update();
    }

    {
        ImPlotStyle& style = ImPlot::GetStyle();
        style.PlotPadding = ImVec2(0, 0);

        static ImPlotAxisFlags xflags = ImPlotAxisFlags_AutoFit
                                      | ImPlotAxisFlags_NoLabel
                                      | ImPlotAxisFlags_NoTickLabels;
        static ImPlotAxisFlags yflags = ImPlotAxisFlags_NoLabel
                                      | ImPlotAxisFlags_NoTickLabels
                                      | ImPlotAxisFlags_NoTickMarks;

        if (ImPlot::BeginPlot("##Line Plots", ImVec2(ImGui::CalcItemWidth(), 100))) {
            ImGui::Text("Hello!!");
            ImPlot::SetupAxes("iterations", "y", xflags, yflags);
            for (Fn& channel : channels) {
                static float xs1[1001], ys1[1001];
                for (int i = 0; i < num_colors; ++i) {
                    xs1[i] = i;
                    ys1[i] = channel(reverse ? num_colors - i - 1 : i);
                }
                ImPlot::SetNextLineStyle(ImVec4(channel.r, channel.g, channel.b, 1));
                ImPlot::PlotLine(channel.label.c_str(), xs1, ys1, num_colors);
                // ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
                // ImPlot::PlotLine("g(x)", xs2, ys2, 20,ImPlotLineFlags_Segments);

            }
            ImPlot::EndPlot();
        }
    }
    
    // TODO: tooltip
}

Fn::Fn(std::string label, float r, float g, float b) : r(r) , g(g) , b(b) {}

float Fn::operator()(float iteration) {
    return 0.5f + 0.5f * glm::cos(x_offset + iteration * x_scale);
}
