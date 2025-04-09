#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "mandelbrot/shader.h"
#include "fragment.frag"
#include "vertex.vert"

// #include <SDL2/SDL.h>
// #include <SDL2/SDL_mixer.h>
// #include "mandelbrot/mystical.h" // music file

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
bool is_pressed(GLFWwindow* window, int key);

int width = 1000;
int height = 1000;

std::string loadShaderSource(const char* filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filepath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

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
    GLuint vertexShader = compile_shader_from_string(GL_VERTEX_SHADER, vertex_shader_str);
    GLuint fragmentShader = compile_shader_from_string(GL_FRAGMENT_SHADER, fragment_shader_str);
    if (vertexShader == -1 || fragmentShader == -1) return -1;
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);
    
    /* quad */
    // with border
    // float vertices[] = {
    //     -0.95f, -0.95f, 0.0f, // bl
    //     -0.95f,  0.95f, 0.0f, // tl
    //      0.95f,  0.95f, 0.0f, // tr
    //      0.95f, -0.95f, 0.0f, // br
    // };
    // no border
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
    
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.12f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer
        
        glUseProgram(shaderProgram);

        double zoom_speed = is_pressed(window, GLFW_KEY_SPACE) ? 1.04 : 1.01;
        double pan_speed = zoom * (is_pressed(window, GLFW_KEY_SPACE) ? 0.05 : 0.02);
        
        if (is_pressed(window, GLFW_KEY_W)) camera_y += pan_speed;
        if (is_pressed(window, GLFW_KEY_A)) camera_x -= pan_speed;
        if (is_pressed(window, GLFW_KEY_S)) camera_y -= pan_speed;
        if (is_pressed(window, GLFW_KEY_D)) camera_x += pan_speed;
        if (is_pressed(window, GLFW_KEY_Q)) zoom *= zoom_speed;
        if (is_pressed(window, GLFW_KEY_E)) zoom /= zoom_speed;
        if (is_pressed(window, GLFW_KEY_ESCAPE)) break;

        glUniform2f(glGetUniformLocation(shaderProgram, "resolution"), width, height);
        glUniform1f(glGetUniformLocation(shaderProgram, "time"), glfwGetTime());
        glUniform2d(glGetUniformLocation(shaderProgram, "camera"), camera_x, camera_y);
        glUniform1d(glGetUniformLocation(shaderProgram, "zoom"), zoom);

        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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