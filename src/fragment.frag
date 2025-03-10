const char* fragment_shader_str = R"(

#version 460 core
out vec4 FragColor;
in vec3 pos;

uniform float time;

int mandel(float z_r, float z_i, float c_r, float c_i) {
    for (int i = 0; i < 49; i++) {
        float z_r_new = z_r*z_r - z_i*z_i    + c_r;
        z_i = 2*z_r*z_i                      + c_i;
        z_r = z_r_new;
        if (z_r > 2 || z_i > 2) {
            return i;
        }
    }
    return 50;
}

void main() {
    vec3 co = pos * 2.0 - 0.3;
    // vec3 co = pos / (0.5 + 0.05*time) - 0.3;
    float m = float(mandel(0, 0, co.x, co.y));
    m /= 50.0;
    FragColor = vec4(m, m, m, 1.0);
}

)";