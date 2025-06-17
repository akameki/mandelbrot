#pragma once

#include <glad/glad.h>

class FrameBuffer {
public:
    enum class Format {
        RGB8,
        R32F,
    };

    GLuint id;
    GLuint texture_id;
    GLuint renderbuffer_id;

    FrameBuffer(int width, int height, Format format, bool with_depth_stencil);
    FrameBuffer(const FrameBuffer& other) = delete;
    FrameBuffer(FrameBuffer&& other) = delete;
    FrameBuffer& operator=(const FrameBuffer& other) = delete;
    FrameBuffer& operator=(FrameBuffer&& other) = delete;
    ~FrameBuffer();

    void rescale(int width, int height);
    void bind();
    void unbind();
private:
    int width, height;
    bool with_depth_stencil;
    GLenum m_format, m_format_enum, m_type;
};
