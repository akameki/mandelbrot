const char* passthrough_fragment_shader_str = R"(

#version 460 core

in vec2 pos;
in vec2 tex;

out vec4 FragColor;

uniform sampler2D superTexture;

void main() {


    FragColor = texture(superTexture, tex);
    // FragColor = vec4(tex.x, tex.y, 0.0, 1.0); // debug
}

)";
