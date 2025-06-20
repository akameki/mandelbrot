#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include "imgui.h"
#include "implot.h"

#include <vector>
#include <string>

#include "palette.h"
#include "constants.h"


Palette::Palette(PaletteState* state) : state(state) {
    glGenTextures(1, &texture);
    bind_texture();

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    float border_color[] = { 1.0f, 0.0f, 0.0f, 1.0f }; // to catch bugs
    glTexParameterfv(GL_TEXTURE_1D, GL_TEXTURE_BORDER_COLOR, border_color);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    // channels = std::vector<ChannelFn>{Fn("r", 1,0,0), Fn("g", 0,1,0), Fn("b", 0,0,1)};
    // create color channels

    // TODO: figure out where this logic show go?
    if (state->channels.empty()) {
        state->channels["red"].color.x = 1.0f;
        state->channels["red"].y_scale = 0.8f;
        state->channels["red"].x_scale = 0.013f;
        state->channels["red"].x_offset = 0.0f;
        state->channels["green"].color.y = 1.0f;
        state->channels["green"].y_scale = 0.2f;
        state->channels["green"].x_offset = PI/3.0f;
        state->channels["green"].x_scale = 0.033f;
        state->channels["blue"].color.z = 1.0f;
        state->channels["blue"].x_offset = 2.0f*PI/3.0f;
        state->channels["blue"].x_scale = 2.5f;
        state->channels["blue"].y_scale = 1.0f;
    }

    // Set the palette colors
    // generate();
    // glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, state->num_colors, 0, GL_RGB, GL_FLOAT, colors.data());
    unbind_texture();
}

void Palette::reverse() { state->reversed = !state->reversed; }
void Palette::update_filter() {
    bind_texture();
    if (state->use_smooth) {
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    unbind_texture();
}

void Palette::generate(int num_colors) {
    colors.resize(num_colors);
    for (int i = 0; i < num_colors; ++i) {
        colors[state->reversed ? num_colors-1 - i : i] = {
            state->channels["red"](i),
            state->channels["green"](i),
            state->channels["blue"](i)
        };
    }
    if (state->override) colors.back() = state->override_color;
    bind_texture();
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB, num_colors, 0, GL_RGB, GL_FLOAT, colors.data());
    unbind_texture();
}

void Palette::bind_texture() { glBindTexture(GL_TEXTURE_1D, texture); }
void Palette::unbind_texture() { glBindTexture(GL_TEXTURE_1D, 0); }

void Palette::draw_ui() {
    ImGui::SeparatorText("Palette");
    // Palette preview
    {
        // perhaps one day, i will figure out to directly draw a 1d texture.
        // {
        //     const float p_width = ImGui::CalcItemWidth();
        //     const float p_height = ImGui::GetFrameHeight();
        //     ImGui::Image((ImTextureID)texture, {p_width, p_height}, ImVec2(0,1), ImVec2(1,0));
        // }
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        float height = ImGui::GetFrameHeight();
        ImVec2 rect_size(ImGui::CalcItemWidth(), height);
        ImVec2 p0 = ImGui::GetCursorScreenPos(); // top left
        ImVec2 p1 = ImVec2(p0.x + rect_size.x, p0.y + rect_size.y); // bottom right
        
        for (int i = 0; i < colors.size(); i++) {
            glm::vec3 col = colors[i];
            ImVec2 rect_p0(p0.x + (rect_size.x * i/colors.size()), p0.y);
            ImVec2 rect_p1(p0.x + (rect_size.x * (i+1)/colors.size()), p1.y);
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
        static ImPlotFlags plotFlags = ImPlotFlags_NoLegend;

        if (ImPlot::BeginPlot("##Line Plots", ImVec2(ImGui::CalcItemWidth(), 50), plotFlags)) {
            ImGui::Text("Hello!!");
            ImPlot::SetupAxes("iterations", "y", xflags, yflags);
            // move to generate()? but we'd have to store all values in memory then..?
            for (auto& [name, channel] : state->channels) {
                static float xs1[1001], ys1[1001];
                for (int i = 0; i < colors.size(); ++i) {
                    xs1[i] = i;
                    ys1[i] = channel(state->reversed ? colors.size() - i - 1 : i);
                }
                ImPlot::SetNextLineStyle(ImVec4(channel.color.x, channel.color.y, channel.color.z, 0.75));
                ImPlot::PlotLine(name.c_str(), xs1, ys1, colors.size());

            }
            ImPlot::EndPlot();
        }

        /* Channel Controls */
        for (auto& [name, channel] : state->channels) {
            draw_channel_settings(name, channel);
        }
    
        /* Options */
        if (ImGui::Button("Reverse")) reverse(); ImGui::SameLine();
        ImGui::SameLine();
        ImGui::Text("Set Color:");
        ImGui::SameLine();
        if (state->override) {
            ImGuiColorEditFlags color_edit_flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel;
            ImGui::ColorEdit3("Static Set Color##set_color", (float*)&state->override_color, color_edit_flags);
        } else {
            ImGuiColorEditFlags color_edit_flags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoPicker;
            ImGui::ColorEdit3("Set color##set_color", (float*)&colors.back(), color_edit_flags);
        }
        ImGui::SameLine();
        ImGui::Checkbox("static", &state->override);

        ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
        ImGui::TreePop();
    }
    // TODO: tooltip
}

void Palette::draw_channel_settings(const std::string& name, ChannelState& channel) {
    ImGui::PushID(name.c_str());
    
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 2));
    ImGui::PushItemWidth(40.f);
    
    ImGui::AlignTextToFramePadding();
    ImVec4 color(channel.color.x, channel.color.y, channel.color.z, 1.0f);
    ImGui::ColorButton("##color", color, ImGuiColorEditFlags_NoTooltip, ImVec2(12, 12));
    ImGui::SameLine();
    
    // controls
    ImGui::DragFloat("##yscale", &channel.y_scale, 0.01f, 0.0f, 1.0f, "%4.2f");
    ImGui::SameLine();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("cos(");
    ImGui::SameLine();
    ImGui::DragFloat("##xoff", &channel.x_offset, 0.01f, 0.0f, PI, "%4.2f");
    ImGui::SameLine();
    ImGui::Text(" ");
    ImGui::SameLine();
    ImGui::DragFloat("##xscale", &channel.x_scale, 0.01f, 0.0f, PI, "%4.2f");
    ImGui::SameLine();
    ImGui::Text("i ");
    ImGui::SameLine();
    ImGui::DragFloat("##time", &channel.t_scale, 0.01f, -5.0f, 5.0f, "%+5.2f");
    ImGui::SameLine();
    ImGui::Text("t)");
    ImGui::PopItemWidth();
    
    ImGui::PopStyleVar();
    ImGui::PopID();
}
