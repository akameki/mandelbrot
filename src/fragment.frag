const char* fragment_shader_str = R"(

#version 460 core
out vec4 FragColor;
in vec3 pos;

uniform vec2 resolution;
uniform float time;
uniform dvec2 camera;
uniform double zoom;

uniform int iterations;

/*  Mandelbrot set
 *  The set of complex numbers c,
 *  for which f_c(z) = z^2 + c does not diverge to infinity when iterated.
 *  i.e. when f(f(...f(0))) remains bounded.
*/

dvec2 square_complex(dvec2 complex) {
    dvec2 res;
    res.x = (complex.x * complex.x - complex.y * complex.y);
    res.y = 2 * complex.x * complex.y;
    return res;
}

vec4 color(int i) {
    float pct = 1 - i / float(iterations);
    // vec3 pd = vec3(0.9, 1.3, 1.1);
    vec3 pd = 5 * vec3(1.3, 1.1, 0.9); // black-blue
    // vec3 pd = vec3(10.28, 11.0, 9.35);
    return vec4(0.5-(cos(pct * pd))/2, 1.0);
}

// returns: # iterations to reach escape condition, capped at 80.
int mandel(dvec2 z, dvec2 c) {
    for (int i = 0; i < iterations; i++) {
        if (length(z) > 2) {
            return i;
        }
        z = square_complex(z) + c;
        // escape condition: modulus > 2
    }
    return iterations;
}

void main() {
    // dvec2 coords = pos.xy * zoom + camera;
    dvec2 coords = camera + dvec2(pos.x * zoom * (double(resolution.x) / double(resolution.y)), pos.y * zoom);

    FragColor = color(mandel(dvec2(0.0,0.0), 2 * coords));
    

    // show color palette on bottom:
    // if (pos.y < -0.958) FragColor = vec4(0.7,0.7,0.7,1.0);
    // if (pos.y < -0.96) FragColor = color(iterations - int(0.5 * iterations * (pos.x + 1.0)));
}

)";
