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

#include "fragment.frag"
#include "vertex.vert"

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

    /* shader program */
    if (!app.shader_program.attach_from_string(GL_VERTEX_SHADER, vertex_shader_str)) return -1;
    if (!app.shader_program.attach_from_string(GL_FRAGMENT_SHADER, fragment_shader_str)) return -1;
    
    app.shader_program.link();
    app.shader_program.use();

    // framebuffer to render into.
    // currently not in use.
    // FrameBuffer framebuffer{width, height};

    float vertices[] = {
        -1.0f, -1.0f, 0.0f, // bl
        -1.0f,  1.0f, 0.0f, // tl
         1.0f,  1.0f, 0.0f, // tr
         1.0f, -1.0f, 0.0f, // br
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glUniform1i(app.shader_program.uniform_location("iterations"), iterations);

    
    // std::thread renderer(renderThread, &app);

    // glfwSwapInterval(0); // disable vsync

    Palette palette(iterations + 1);
    
    while (!glfwWindowShouldClose(app.window)) {
        glfwPollEvents();             // Process events
        
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
            }
            palette.draw_ui();
            imgui_camera_ui();
            ImGui::Separator();
            ImGui::Text("%.1f FPS", imGuiIO.Framerate);
            
            ImGui::End();
        }
        
        update_options(app.window);

        {
            update_uniforms(app.window, app.shader_program);
            glClearColor(0.12f, 0.1f, 0.12f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer
            
            if (is_pressed(app.window, GLFW_KEY_ESCAPE)) break;
            
            app.shader_program.use();
            palette.update();
            palette.bind();
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
    glViewport(0, 0, width, height);
}
