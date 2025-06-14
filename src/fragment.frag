const char* fragment_shader_str = R"(

#version 460 core
out vec4 FragColor;
in vec3 pos;

uniform vec2 resolution;
uniform float time;
uniform dvec2 camera;
uniform double zoom;

uniform int iterations;

uniform sampler1D palette;

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

// vec4 color(int i) {
//     float pct = 1 - i / float(iterations);
//     // vec3 pd = vec3(0.9, 1.3, 1.1);
//     vec3 pd = 5 * vec3(1.3, 1.1, 0.9); // black-blue
//     // vec3 pd = vec3(10.28, 11.0, 9.35);
//     return vec4(0.5-(cos(pct * pd))/2, 1.0);
// }

// returns: # iterations to reach escape condition
// for true mandelbrot, the parameter z is always 0, it's only ever modified within the loop.
int mandel(dvec2 z, dvec2 c) {
    for (int i = 1; i <= iterations; i++) {
        // escape condition: modulus > 2
        z = square_complex(z) + c;
        if (length(z) > 2) {
            return i;
        }
    }
    return iterations + 1;
}

void main() {
    double aspect = double(resolution.x) / double(resolution.y);
    dvec2 coords = camera + dvec2(
        pos.x * 1.0/zoom * aspect, 
        pos.y * 1.0/zoom
    );
    // mandel returns 1 ~ iterations+1
    float t = mandel(dvec2(0.0,0.0), coords) - 1;
    FragColor = texture(palette, t / float(iterations+1) + 0.000001);
}
    // show color palette on bottom:
    // if (pos.y < -0.958) FragColor = vec4(0.7,0.7,0.7,1.0);
    // if (pos.y < -0.96) FragColor = texture(palette, (0.5 * (pos.x + 1.0)));

)";
