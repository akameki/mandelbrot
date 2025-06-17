#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"

#include <iostream>
#include <thread>

#include "shader.h"
#include "framebuffer.h"
#include "palette.h"
#include "settings.h"

#include "shaders/vertex.vert"
#include "shaders/fractal_pass.frag"
#include "shaders/palette_pass.frag"
#include "shaders/downsample_pass.frag"

// #include <SDL2/SDL.h>
// #include <SDL2/SDL_mixer.h>
// #include "mandelbrot/mystical.h" // music file

void framebuffer_size_callback(GLFWwindow* window, int width, int height);


#ifdef _WIN32
#include <windows.h>
int main(int argc, char** argv);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    char* argv[] = { nullptr };
    return main(0, argv);
}
#endif

class App {
public:
    GLFWwindow* window;
    ShaderProgram shader_program;
    // Palette palette;

    // App() {}
};

// void renderThread(App* app) {
//     glfwMakeContextCurrent(app->window);
//     while (!glfwWindowShouldClose(app->window)) {
//         update_uniforms(app->window, app->shader_program);
//         glClearColor(0.12f, 0.1f, 0.12f, 1.0f);
//         glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer
        
//         if (is_pressed(app->window, GLFW_KEY_ESCAPE)) break;
        
//         app->shader_program.use();
//         app->palette.bind();
        
//         glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//         // glfwSwapBuffers(app->window);      // Swap front and back buffers
//     }
// }

int main(int argc, char** argv) {
    /* GLFW */
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    GLFWwindow* window = glfwCreateWindow(width, height, "Mandelbrot Explorer", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    /* GLAD */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    /* ImGui */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& imGuiIO = ImGui::GetIO(); (void)imGuiIO;
    imGuiIO.IniFilename = NULL;
    imGuiIO.LogFilename = NULL;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    /* SDL2 */
    // if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    //     std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    //     return -1;
    // } else if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    //     std::cerr << "Mix_OpenAudio Error: " << Mix_GetError() << std::endl;
    //     SDL_Quit();
    //     return -1;
    // }
    // SDL_RWops* rw = SDL_RWFromMem(mystical_ogg, mystical_ogg_len);
    // Mix_Music* music = Mix_LoadMUS_RW(rw, 1);
    // Mix_PlayMusic(music, -1);

    App app;
    app.window = window;

    /* fractal iterator shader */
    if (!app.shader_program.attach_from_string(GL_VERTEX_SHADER, vertex_shader_str)) return -1;
    if (!app.shader_program.attach_from_string(GL_FRAGMENT_SHADER, fractal_pass_fragment_str)) return -1;
    app.shader_program.link();
    app.shader_program.use();
    
    /* palette shader */
    ShaderProgram paletteShader;
    if (!paletteShader.attach_from_string(GL_VERTEX_SHADER, vertex_shader_str)) return -1;
    if (!paletteShader.attach_from_string(GL_FRAGMENT_SHADER, palette_pass_shader_str)) return -1;
    paletteShader.link();
    paletteShader.use();

    /* SSAA shader */
    ShaderProgram ssaaShader;
    if (!ssaaShader.attach_from_string(GL_VERTEX_SHADER, vertex_shader_str)) return -1;
    if (!ssaaShader.attach_from_string(GL_FRAGMENT_SHADER, passthrough_fragment_shader_str)) return -1;
    ssaaShader.link();
    ssaaShader.use();

    float vertices[] = {
      // pos           // tex
        -1.0f, -1.0f,     0.0f, 0.0f, // bl
        -1.0f,  1.0f,     0.0f, 1.0f, // tl
         1.0f,  1.0f,     1.0f, 1.0f, // tr
         1.0f, -1.0f,     1.0f, 0.0f, // br
    };
    unsigned int indices[] = { 0, 1, 2,  0, 2, 3 };

    GLuint VBO, VAO, EBO;
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));

    glUniform1i(app.shader_program.uniform_location("iterations"), iterations);

    
    // std::thread renderer(renderThread, &app);

    // glfwSwapInterval(0); // disable vsync

    // framebuffer for SSAA.
    // should eventually be used for storing fractal when iterations/camera doesn't change.
    FrameBuffer iterFramebuffer(width * 2, height * 2, FrameBuffer::Format::R32F, false);
    FrameBuffer coloredFramebuffer{width * 2, height * 2, FrameBuffer::Format::RGB8, false};

    Palette palette(iterations + 1);
    
    while (!glfwWindowShouldClose(app.window)) {
        glfwPollEvents();             // Process events
        
        if (is_pressed(app.window, GLFW_KEY_ESCAPE)) break;

        app.shader_program.use();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        bool show_demo_window = true;
        // ImGui::ShowDemoWindow(&show_demo_window);
        if (show_ui) {
            ImGui::Begin("Mandelbrot");
            // ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImGui::PushItemWidth(-FLT_MIN);
            if (ImGui::SliderInt("##iterations", &iterations, 1, 500, "Iterations = %d")) {
                palette.resize(iterations + 1);
                recalculate_fractal = true;
            }
            palette.draw_ui();
            imgui_camera_ui();
            ImGui::SeparatorText("Graphics");
            if (ImGui::Checkbox("SSAA", &ssaa_toggle)) {
                recalculate_fractal = true;
            };
            ImGui::Separator();
            ImGui::Text("%.1f FPS", imGuiIO.Framerate);
            
            ImGui::End();
        }
        
        update_camera(app.window);
        palette.update();

        {
            update_uniforms(app.window, app.shader_program);
            glClearColor(0.12f, 0.1f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            
            int ssaa_factor = ssaa_toggle ? 2 : 1;
            
            // first pass (iterations)
            if (recalculate_fractal) {
                recalculate_fractal = false;
                iterFramebuffer.rescale(width*ssaa_factor, height*ssaa_factor); // TODO: move to callback
                iterFramebuffer.bind();
                glViewport(0, 0, width*ssaa_factor, height*ssaa_factor);
                app.shader_program.use();
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
            // second pass (color)
            coloredFramebuffer.rescale(width*ssaa_factor, height*ssaa_factor);
            coloredFramebuffer.bind();
            glClear(GL_COLOR_BUFFER_BIT);
            glViewport(0, 0, width*ssaa_factor, height*ssaa_factor);
            paletteShader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, iterFramebuffer.texture_id);
            glActiveTexture(GL_TEXTURE1);
            palette.bind();
            glUniform1i(paletteShader.uniform_location("iterTex"), 0);
            glUniform1i(paletteShader.uniform_location("paletteTex"), 1);
            glUniform1i(paletteShader.uniform_location("iterations"), iterations);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            
            // thirds pass (downsample, sometimes)
            coloredFramebuffer.unbind();
            glClear(GL_COLOR_BUFFER_BIT);
            glViewport(0, 0, width, height);
            ssaaShader.use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, coloredFramebuffer.texture_id);
            glUniform1i(ssaaShader.uniform_location("superTexture"), 0);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(app.window);      // Swap front and back buffers
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwTerminate(); // Clean up GLFW resources
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int w, int h) {
    width = w;
    height = h;
    recalculate_fractal = true;
    // glViewport(0, 0, width, height);
}
