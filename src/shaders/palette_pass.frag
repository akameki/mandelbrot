const char* palette_pass_shader_str = R"(

#version 460 core

in vec2 pos;
in vec2 tex;

uniform sampler2D iterTex;  // R32F
uniform sampler1D paletteTex;       // RGB or RGBA
uniform int iterations;

out vec4 FragColor;

// uniform sampler2D superTexture;

void main() {
    float iter = texture(iterTex, tex).r;

    float t = (iter - 0.5) / float(iterations + 1);
    FragColor = texture(paletteTex, t);
    // vec4 color = texture(paletteTex, t);

    // FragColor = vec4(t, t, t, 1.0); // debug
    // FragColor = vec4(pos.x, pos.y, iter, 1.0); // debug
}

)";
