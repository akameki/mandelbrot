const char* vertex_shader_str = R"(

#version 460 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTex;
out vec2 pos;
out vec2 tex;
void main() {
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    pos = aPos;
    tex = aTex;
}

)";
