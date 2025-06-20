#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <thread>
#include <filesystem>
#include <cstdlib>

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

// void renderThread(DepraApp* app) {
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
    App app;
    AppState& state = app.state;
    load_state(app, std::filesystem::absolute("mandelconfig"));
    /* GLFW */
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    app.window = glfwCreateWindow(state.width, state.height, "Mandelbrot Explorer", nullptr, nullptr);
    glfwSetWindowUserPointer(app.window, &app);
    if (!app.window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(app.window);
    glfwSetFramebufferSizeCallback(app.window, framebuffer_size_callback);
    glfwSetKeyCallback(app.window, key_callback);
    glfwSetMouseButtonCallback(app.window, mouse_button_callback);
    glfwSetScrollCallback(app.window, scroll_callback);

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
    ImGui_ImplGlfw_InitForOpenGL(app.window, true);
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

    ShaderProgram fractal_shader;
    if (!fractal_shader.attach_from_string(GL_VERTEX_SHADER, vertex_shader_str)) return -1;
    if (!fractal_shader.attach_from_string(GL_FRAGMENT_SHADER, fractal_pass_fragment_str)) return -1;
    fractal_shader.link();
    fractal_shader.use();
    
    ShaderProgram palette_shader;
    if (!palette_shader.attach_from_string(GL_VERTEX_SHADER, vertex_shader_str)) return -1;
    if (!palette_shader.attach_from_string(GL_FRAGMENT_SHADER, palette_pass_shader_str)) return -1;
    palette_shader.link();
    palette_shader.use();

    ShaderProgram passthrough_shader;
    if (!passthrough_shader.attach_from_string(GL_VERTEX_SHADER, vertex_shader_str)) return -1;
    if (!passthrough_shader.attach_from_string(GL_FRAGMENT_SHADER, passthrough_fragment_shader_str)) return -1;
    passthrough_shader.link();
    passthrough_shader.use();

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

    glUniform1i(fractal_shader.uniform_location("iterations"), state.max_iterations);

    
    // std::thread renderer(renderThread, &app);

    // glfwSwapInterval(0); // disable vsync

    FrameBuffer fractal_fbuffer(state.width * 2, state.height * 2, FrameBuffer::Format::R32F, false);
    FrameBuffer paletted_fbuffer{state.width * 2, state.height * 2, FrameBuffer::Format::RGB8, false};

    Palette palette(&app.state.palette_state);
    palette.update_filter();
    
    while (!glfwWindowShouldClose(app.window)) {
        glfwPollEvents();             // Process events
        
        if (is_pressed(app.window, GLFW_KEY_ESCAPE)) break;

        fractal_shader.use();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        bool show_demo_window = true;
        // ImGui::ShowDemoWindow(&show_demo_window);
        if (state.show_ui) {
            ImGui::Begin("Mandelbrot");
            // ImDrawList* draw_list = ImGui::GetWindowDrawList();
            ImGui::PushItemWidth(-FLT_MIN);
            if (ImGui::SliderInt("##iterations", &state.max_iterations, 1, 500, "Iterations = %d")) state.dirty_fractal = true;
            palette.draw_ui();
            imgui_camera_ui(app);
            ImGui::SeparatorText("Graphics");
            if (ImGui::Checkbox("SSAA", &state.use_ssaa)) state.dirty_fractal = true;
            ImGui::SameLine();
            if (ImGui::Checkbox("Smooth coloring", &state.palette_state.use_smooth)) palette.update_filter();
            ImGui::Separator();
            ImGui::Text("%.1f FPS", imGuiIO.Framerate);
            
            ImGui::End();
        }
        
        update_camera(app);
        palette.generate(state.max_iterations + 1);
        update_uniforms(app, fractal_shader);

        {
            glClearColor(0.12f, 0.1f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            
            int fractal_res_width = state.width;
            int fractal_res_height = state.height;
            if (state.use_ssaa) {
                fractal_res_width *= 2;
                fractal_res_height *= 2;
            }
            
            // first pass (iterations)
            if (state.dirty_fractal) {
                state.dirty_fractal = false;
                fractal_fbuffer.resize(fractal_res_width, fractal_res_height); // TODO: move to callback
                fractal_fbuffer.bind();
                glViewport(0, 0, fractal_res_width, fractal_res_height);
                fractal_shader.use();
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
            // second pass (color)
            paletted_fbuffer.resize(fractal_res_width, fractal_res_height);
            paletted_fbuffer.bind();
            glClear(GL_COLOR_BUFFER_BIT);
            glViewport(0, 0, fractal_res_width, fractal_res_height);
            palette_shader.use();
            glActiveTexture(GL_TEXTURE0);
            fractal_fbuffer.bind_texture();
            glActiveTexture(GL_TEXTURE1);
            palette.bind_texture();
            glUniform1i(palette_shader.uniform_location("iterTex"), 0);
            glUniform1i(palette_shader.uniform_location("paletteTex"), 1);
            glUniform1i(palette_shader.uniform_location("iterations"), state.max_iterations);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            
            // thirds pass (downsample, sometimes)
            paletted_fbuffer.unbind();
            glClear(GL_COLOR_BUFFER_BIT);
            glViewport(0, 0, state.width, state.height);
            passthrough_shader.use();
            glActiveTexture(GL_TEXTURE0);
            paletted_fbuffer.bind_texture();
            glUniform1i(passthrough_shader.uniform_location("superTexture"), 0);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(app.window);      // Swap front and back buffers
    }

    save_state(app, std::filesystem::absolute("mandelconfig"));

    if (true) {
        fractal_fbuffer.resize(1920*2, 1080*2);
        paletted_fbuffer.resize(1920*2, 1080*2);
        glViewport(0, 0, 1920*2, 1080*2);
        fractal_fbuffer.bind();
        fractal_shader.use();
        glUniform2f(fractal_shader.uniform_location("resolution"), 16, 9);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // second pass (color)
        paletted_fbuffer.bind();
        glClear(GL_COLOR_BUFFER_BIT);
        palette_shader.use();
        glActiveTexture(GL_TEXTURE0);
        fractal_fbuffer.bind_texture();
        glActiveTexture(GL_TEXTURE1);
        palette.bind_texture();
        glUniform1i(palette_shader.uniform_location("iterTex"), 0);
        glUniform1i(palette_shader.uniform_location("paletteTex"), 1);
        glUniform1i(palette_shader.uniform_location("iterations"), state.max_iterations);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        std::vector<unsigned char> pixels(1920*2*1080*2*3);
        glReadBuffer(GL_COLOR_ATTACHMENT0); 
        glReadPixels(0, 0, 1920*2, 1080*2, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
        stbi_flip_vertically_on_write(1);
        stbi_write_png("/home/aki/.config/mandelpaper.png", 1920*2, 1080*2, 3, pixels.data(), 1920*2 * 3);

        // std::system("hyprshot -m window -m active -o ~/.config -f mandelpaper");
        std::system("waypaper --wallpaper ~/.config/mandelpaper.png");
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwTerminate(); // Clean up GLFW resources
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int w, int h) {
    App* app = static_cast<App*>(glfwGetWindowUserPointer(window));
    AppState& state = app->state;
    state.width = w;
    state.height = h;
    state.dirty_fractal = true;
    // glViewport(0, 0, width, height);
}
