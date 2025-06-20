const char* fractal_pass_fragment_str = R"(

#version 460 core
in vec2 pos;

uniform vec2 resolution;
uniform float time;
uniform dvec2 camera;
uniform double zoom;

uniform int iterations;

// out vec4 FragColor;
layout(location = 0) out float escapeIter;

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

// returns # iterations to reach escape condition
// for mandelbrot, the parameter z should be (0,0).
float mandel(dvec2 z, dvec2 c) {
    for (int i = 1; i <= iterations; i++) {
        z = square_complex(z) + c;

        if (length(z) > (1<<16)) {
            float log_zn = log(float(z.x*z.x + z.y*z.y)) / 2.0;
            float nu = log(log_zn / log(2.0)) / log(2.0);
            return i + 1.0 - nu;
        }
    }
    return iterations + 1; // no bailout; is in the set
}

void main() {
    double aspect = double(resolution.x) / double(resolution.y);
    dvec2 coords = camera + dvec2(
        pos.x * 1.0/zoom * aspect, 
        pos.y * 1.0/zoom
    );
    // mandel returns 1 ~ iterations+1
    escapeIter = mandel(dvec2(0.0,0.0), coords);
    // FragColor = texture(palette, t / float(iterations+1) + 0.000001);
    // FragColor = vec4(t / (iterations + 1));
}
    // show color palette on bottom:
    // if (pos.y < -0.958) FragColor = vec4(0.7,0.7,0.7,1.0);
    // if (pos.y < -0.96) FragColor = texture(palette, (0.5 * (pos.x + 1.0)));

)";
