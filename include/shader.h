#pragma once

#include <glad/glad.h>

class ShaderProgram {
public:
    GLuint id;

    ShaderProgram();
    ShaderProgram(ShaderProgram&& other) = delete;
    ShaderProgram(const ShaderProgram& other) = delete;
    ShaderProgram& operator=(ShaderProgram&& other) = delete;
    ShaderProgram& operator=(const ShaderProgram& other) = delete;
    ~ShaderProgram();

    bool attach_from_string(GLenum shader_type, const char* source);
    void link();
    void use();

    GLint uniform_location(const GLchar *name);
};
