#ifndef MANDEL_SHADER_H
#define MANDEL_SHADER_H

#include <glad/glad.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


GLuint compile_shader_from_string(GLenum shader_type, const char* source) {
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Compiling " << (shader_type == GL_VERTEX_SHADER ? "vertex shader " : "fragment shader ") << "failed\n" << infoLog << std::endl;
        return -1;
    }

    return shader;
}

GLuint compile_shader_from_file(GLenum shader_type, const char* filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filepath << std::endl;
        return -1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();

    const char* shader_source = buffer.str().c_str();
    return compile_shader_from_string(shader_type, shader_source);

}

#endif