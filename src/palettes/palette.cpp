#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include "imgui.h"
#include "implot.h"

#include <vector>
#include <string>

#include "palette.h"
#include "constants.h"

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
    const int generated = num_colors;
    for (int i = 0; i < generated; ++i) {
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
        
        const int generated = num_colors - 1;
        for (int i = 0; i < generated; i++) {
            glm::vec3 col = colors[i];
            ImVec2 rect_p0(p0.x + (rect_size.x * i/generated), p0.y);
            ImVec2 rect_p1(p0.x + (rect_size.x * (i+1)/generated), p1.y);
            draw_list->AddRectFilled(rect_p0, rect_p1, IM_COL32(col.r*255, col.g*255, col.b*255, 255));
        }
        ImGui::InvisibleButton("##palette0", rect_size);
    }
    if (ImGui::TreeNodeEx("Edit", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
        /* Channel graph */
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

        /* Channel Controls */
        for (Fn& channel : channels) {
            channel.draw_ui();
        }
    
        /* Options */
        if (ImGui::Button("Reverse")) reverse(); ImGui::SameLine();
        ImGui::SameLine();
        ImGui::Text("Set Color:");
        ImGui::SameLine();
        if (override) {
            ImGuiColorEditFlags color_edit_flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel;
            if (ImGui::ColorEdit3("Static Set Color##set_color", (float*)&override_color, color_edit_flags)) update();
        } else {
            ImGuiColorEditFlags color_edit_flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoPicker;
            if (ImGui::ColorEdit3("Set color##set_color", (float*)&colors.back(), color_edit_flags)) update();
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("static", &override)) update();

        ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
        ImGui::TreePop();
    }
    // TODO: tooltip
}

Fn::Fn(std::string label, float r, float g, float b) : r(r) , g(g) , b(b) {}

float Fn::operator()(float iteration) {
    return 0.5f + 0.5f * glm::cos(x_offset + iteration * x_scale + t_scale * ImGui::GetTime());
}

// void Fn::draw_ui() {
//     ImGui::PushID(this); // Ensure unique IDs for each channel
    
//     // Channel color indicator and toggle
//     ImVec4 color(r, g, b, 1.0f);
//     ImGui::ColorButton(label.c_str(), color, ImGuiColorEditFlags_NoTooltip, ImVec2(15,15));
//     ImGui::SameLine();
//     if (ImGui::Button(("hi" + label).c_str())) {
//         show_controls = !show_controls;
//     }
    
//     if (show_controls) {
//         ImGui::Indent();
        
//         // Function display
//         ImGui::Text("cos(%.3f + %.3fx + %.3ft)", x_offset, x_scale, t_scale);
        
//         // Parameter controls
//         ImGui::PushItemWidth(100);
//         ImGui::DragFloat("X Offset", &x_offset, 0.01f, -10.0f, 10.0f, "%.3f");
//         ImGui::SameLine();
//         ImGui::DragFloat("X Scale", &x_scale, 0.001f, -1.0f, 1.0f, "%.3f");
//         ImGui::SameLine();
//         ImGui::DragFloat("Time", &t_scale, 0.01f, -10.0f, 10.0f, "%.3f");
//         ImGui::PopItemWidth();
        
//         ImGui::Unindent();
//     }
    
//     ImGui::PopID();
// }

void Fn::draw_ui() {
    ImGui::PushID(this);
    
    // Compact color channel controls
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 2));
    ImGui::PushItemWidth(40.f);
    
    // Color indicator button (fixed width)
    ImGui::AlignTextToFramePadding();
    ImVec4 color(r, g, b, 1.0f);
    ImGui::ColorButton("##color", color, ImGuiColorEditFlags_NoTooltip, ImVec2(12, 12));
    ImGui::SameLine();
    
    // Compact input controls
    ImGui::AlignTextToFramePadding();
    ImGui::Text("cos(");
    ImGui::SameLine();
    ImGui::DragFloat("##xoff", &x_offset, 0.01f, 0, PI, "%4.2f");
    ImGui::SameLine();
    ImGui::Text(" ");
    ImGui::SameLine();
    ImGui::DragFloat("##xscale", &x_scale, 0.001f, -1.f, 1.f, "%+5.2f");
    ImGui::SameLine();
    ImGui::Text("i ");
    ImGui::SameLine();
    ImGui::DragFloat("##time", &t_scale, 0.01f, -10.f, 10.f, "%+5.2f");
    ImGui::SameLine();
    ImGui::Text("t)");
    ImGui::PopItemWidth();
    
    ImGui::PopStyleVar();
    ImGui::PopID();
}
