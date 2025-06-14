#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include "imgui.h"
#include "implot.h"

#include <vector>
#include <string>

#include "palette.h"

Palette::Palette(int size) : num_colors(size) , reversed(false) {
    glGenTextures(1, &texture);
    bind();

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float border_color[] = { 1.0f, 0.0f, 0.0f, 1.0f }; // to catch bugs
    glTexParameterfv(GL_TEXTURE_1D, GL_TEXTURE_BORDER_COLOR, border_color);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);

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

void Palette::reverse() {
    reversed = !reversed;
    update();
}

void Palette::resize(int size) {
    this->num_colors = size;
    update();
}

void Palette::update() {
    colors.resize(num_colors);
    const int generated = num_colors - (int)override;
    for (int i = 0; i < generated; ++i) {
        // const float pct = (float)i / size;
        // const float pct = i * 0.03;
        // glm::vec3 period = 10.0f * glm::vec3{1.3f, 1.1f, 0.9f};
        // colors[reverse ? num_colors - i - 1 : i] = glm::vec3(0.5f - (glm::cos(2.0f + pct * period)) / 2.0f);
        colors[reversed ? generated - i - 1 : i] = {channels[0](i), channels[1](i), channels[2](i)};
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
    ImGui::SeparatorText("Palette");
    // Palette preview
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        float height = ImGui::GetFrameHeight();
        ImVec2 rect_size(ImGui::CalcItemWidth(), height);
        ImVec2 p0 = ImGui::GetCursorScreenPos(); // top left
        ImVec2 p1 = ImVec2(p0.x + rect_size.x, p0.y + rect_size.y); // bottom right
        
        const int generated = num_colors - (int)override;
        for (int i = 0; i < generated; i++) {
            glm::vec3 col = colors[i];
            ImVec2 rect_p0(p0.x + (rect_size.x * i/generated), p0.y);
            ImVec2 rect_p1(p0.x + (rect_size.x * (i+1)/generated), p1.y);
            draw_list->AddRectFilled(rect_p0, rect_p1, IM_COL32(col.r*255, col.g*255, col.b*255, 255));
        }
        ImGui::InvisibleButton("##palette0", rect_size);
    }
    // Palette channels
    {
        ImPlotStyle& style = ImPlot::GetStyle();
        style.PlotPadding = ImVec2(0, 0);

        static ImPlotAxisFlags xflags = ImPlotAxisFlags_AutoFit
                                      | ImPlotAxisFlags_NoLabel
                                      | ImPlotAxisFlags_NoGridLines
                                      | ImPlotAxisFlags_NoTickMarks
                                      | ImPlotAxisFlags_NoTickLabels;
        static ImPlotAxisFlags yflags = ImPlotAxisFlags_NoLabel
                                      | ImPlotAxisFlags_NoTickLabels
                                      | ImPlotAxisFlags_NoGridLines
                                      | ImPlotAxisFlags_Lock
                                      | ImPlotAxisFlags_NoTickMarks;

        if (ImPlot::BeginPlot("##Line Plots", ImVec2(ImGui::CalcItemWidth(), 50))) {
            ImGui::Text("Hello!!");
            ImPlot::SetupAxes("iterations", "y", xflags, yflags);
            // TODO: move to update()
            for (Fn& channel : channels) {
                static float xs1[1001], ys1[1001];
                for (int i = 0; i < num_colors; ++i) {
                    xs1[i] = i;
                    ys1[i] = channel(reversed ? num_colors - i - 1 : i);
                }
                ImPlot::SetNextLineStyle(ImVec4(channel.r, channel.g, channel.b, 0.75));
                ImPlot::PlotLine(channel.label.c_str(), xs1, ys1, num_colors);

            }
            ImPlot::EndPlot();
        }

    if (ImGui::Button("Reverse")) reverse(); ImGui::SameLine();
    if (ImGui::Checkbox("override set color", &override)) update();
    ImGuiColorEditFlags color_edit_flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel;
    ImGui::SameLine();
    if (ImGui::ColorEdit3("##override", (float*)&override_color, color_edit_flags)) update();

    }
    
    // TODO: tooltip
}

Fn::Fn(std::string label, float r, float g, float b) : r(r) , g(g) , b(b) {}

float Fn::operator()(float iteration) {
    return 0.5f + 0.5f * glm::cos(x_offset + iteration * x_scale);
}
