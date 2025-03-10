#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "mandelbrot/shader.h"
#include "fragment.frag"
#include "vertex.vert"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "mandelbrot/mystical.h"

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
    GLFWwindow* window = glfwCreateWindow(1000, 1000, "Simple GLFW Window", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    /* GLAD */
    /* ---- */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    /* SDL2 */
    /* ---- */
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return -1;
    } else if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Mix_OpenAudio Error: " << Mix_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }
    SDL_RWops* rw = SDL_RWFromMem(mystical_ogg, mystical_ogg_len);
    Mix_Music* music = Mix_LoadMUS_RW(rw, 1);
    Mix_PlayMusic(music, -1);


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
    float vertices[] = {
        -0.9f, -0.9f, 0.0f, // bl
        -0.9f,  0.9f, 0.0f, // tl
         0.9f,  0.9f, 0.0f, // tr
         0.9f, -0.9f, 0.0f, // br
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

    
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.12f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

        glUseProgram(shaderProgram);

        GLuint time_loc = glGetUniformLocation(shaderProgram, "time");
        glUniform1f(time_loc, glfwGetTime());

        // glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);      // Swap front and back buffers
        glfwPollEvents();             // Process events
    }

    glfwTerminate(); // Clean up GLFW resources
    return 0;
}