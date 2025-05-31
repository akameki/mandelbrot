#pragma once

#include <glad/glad.h>

class FrameBuffer {
public:
    GLuint id;
    GLuint texture_id;
    GLuint renderbuffer_id;

    FrameBuffer(int width, int height);
    FrameBuffer(const FrameBuffer& other) = delete;
    FrameBuffer(FrameBuffer&& other) = delete;
    FrameBuffer& operator=(const FrameBuffer& other) = delete;
    FrameBuffer& operator=(FrameBuffer&& other) = delete;
    ~FrameBuffer();

    void rescale(int width, int height);
    void bind();
    void unbind();

};
