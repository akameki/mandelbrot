#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>

#include "shader.h"
#include "framebuffer.h"
#include "palette.h"

#include "fragment.frag"
#include "vertex.vert"

// #include <SDL2/SDL.h>
// #include <SDL2/SDL_mixer.h>
// #include "mandelbrot/mystical.h" // music file

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
bool is_pressed(GLFWwindow* window, int key);

int width = 1000;
int height = 1000;

int main(int argv, char** args) {
    /* GLFW */
    /* ---- */
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

    /* GLAD */
    /* ---- */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    /* ImGui */
    /* ----- */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    /* SDL2 */
    /* ---- */
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


    /* OpenGL Shader */
    /* ------------- */
    ShaderProgram shader_program;
    if (!shader_program.attach_from_string(GL_VERTEX_SHADER, vertex_shader_str)) return -1;
    if (!shader_program.attach_from_string(GL_FRAGMENT_SHADER, fragment_shader_str)) return -1;
    
    shader_program.link();
    shader_program.use();
    
    /* quad */
    // with border
    // float vertices[] = {
    //     -0.95f, -0.95f, 0.0f, // bl
    //     -0.95f,  0.95f, 0.0f, // tl
    //      0.95f,  0.95f, 0.0f, // tr
    //      0.95f, -0.95f, 0.0f, // br
    // };
    // no border

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

    double camera_x = -0.3;
    double camera_y = 0.0;
    double zoom = 1.0;
    double pan_speed = 0.02;


    int iterations = 5;
    glUniform1i(shader_program.uniform_location("iterations"), iterations);

    ImVec4 im_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    Palette palette(iterations + 1);
    
    while (!glfwWindowShouldClose(window)) {

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        bool show_demo_window = true;
        // ImGui::ShowDemoWindow(&show_demo_window);

        {
            ImGui::Begin("Mandelbrot");
            static int counter = 0;
            ImGui::Text("Some useful text.");
            if (ImGui::SliderInt("iterations", &iterations, 1, 500)) {
                glUniform1i(shader_program.uniform_location("iterations"), iterations);
                palette.resize(iterations + 1);
            }
            // ImGui::ColorEdit3("color! (nop)", (float*)&im_color);
            
            // ImDrawList* draw_list = ImGui::GetWindowDrawList();

            palette.draw_ui();
            // if (ImGui::Button("press me!")) ++counter;
            // ImGui::SameLine();
            // ImGui::Text("counter = %d", counter);

            ImGui::SeparatorText("Camera");
            ImGui::Text("Camera X=%.7f", camera_x);
            ImGui::Text("Camera Y=%.7f", camera_y);
            ImGui::Text("Zoom=%.7f", zoom);

            ImGui::Separator();
            ImGui::Text("%.1f FPS", io.Framerate);

            ImGui::End();
        }

        // {
        //     ImGui::Begin("Scene");
        //     const float win_width = ImGui::GetContentRegionAvail().x;
        //     const float win_height = ImGui::GetContentRegionAvail().y;
        //     framebuffer.rescale(win_width, win_height);
        //     ImGui::Image((ImTextureID)framebuffer.texture_id, ImGui::GetContentRegionAvail(), ImVec2(0,1), ImVec2(1,0));
        //     ImGui::End();
        // }
        
        
        glClearColor(0.12f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer
        
        
        double zoom_speed = is_pressed(window, GLFW_KEY_SPACE) ? 1.04 : 1.01;
        double pan_speed = zoom * (is_pressed(window, GLFW_KEY_SPACE) ? 0.05 : 0.02);
        
        if (is_pressed(window, GLFW_KEY_W)) camera_y += pan_speed;
        if (is_pressed(window, GLFW_KEY_A)) camera_x -= pan_speed;
        if (is_pressed(window, GLFW_KEY_S)) camera_y -= pan_speed;
        if (is_pressed(window, GLFW_KEY_D)) camera_x += pan_speed;
        if (is_pressed(window, GLFW_KEY_Q)) zoom *= zoom_speed;
        if (is_pressed(window, GLFW_KEY_E)) zoom /= zoom_speed;
        if (is_pressed(window, GLFW_KEY_ESCAPE)) break;
        glUniform1f(shader_program.uniform_location("time"), glfwGetTime());
        glUniform2d(shader_program.uniform_location("camera"), camera_x, camera_y);
        glUniform1d(shader_program.uniform_location("zoom"), zoom);
        glUniform2f(shader_program.uniform_location("resolution"), width, height);

        
        shader_program.use();
        // framebuffer.bind();
        palette.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // framebuffer.unbind();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);      // Swap front and back buffers
        glfwPollEvents();             // Process events
    }

    glfwTerminate(); // Clean up GLFW resources
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int w, int h) {
    width = w;
    height = h;
    glViewport(0, 0, width, height);
}

bool is_pressed(GLFWwindow* window, int key) {
    return glfwGetKey(window, key) != GLFW_RELEASE;
}
