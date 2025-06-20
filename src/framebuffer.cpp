#include "framebuffer.h"

#include <iostream>

FrameBuffer::FrameBuffer(int width, int height, Format format, bool with_depth_stencil = false) : width(width), height(height) {
    switch (format) {
        case Format::RGB8:
            m_format = GL_RGB;
            m_format_enum = GL_RGB;
            m_type = GL_UNSIGNED_BYTE;
            break;
        case Format::R32F:
            m_format = GL_R32F;
            m_format_enum = GL_RED;
            m_type = GL_FLOAT;
            break;
        default:
            throw std::runtime_error("Unknown framebuffer format");
    }
    glGenFramebuffers(1, &id);
    glGenTextures(1, &texture_id);
    bind();
    bind_texture();
    glTexImage2D(GL_TEXTURE_2D, 0, m_format, width, height, 0, m_format_enum, m_type, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);

    // if (with_depth_stencil) {
    //     glGenRenderbuffers(1, &renderbuffer_id);
    //     glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_id);
    //     glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    //     glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer_id);
    //     glBindRenderbuffer(GL_RENDERBUFFER, 0);
    // }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR::FRAMEBUFFER not complete after creating" << std::endl;
    }

    unbind_texture();
    unbind();
}

void FrameBuffer::resize(int new_width, int new_height) {
    bind();
    bind_texture();
    glTexImage2D(GL_TEXTURE_2D, 0, m_format, new_width, new_height, 0, m_format_enum, m_type, NULL);

    
    // if (with_depth_stencil) {
    //     glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_id);
    //     glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, new_width, new_height);
    //     glBindRenderbuffer(GL_RENDERBUFFER, 0);
    // }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR::FRAMEBUFFER:: not complete after resize" << std::endl;
    }

    unbind_texture();
    unbind();
}

void FrameBuffer::bind() { glBindFramebuffer(GL_FRAMEBUFFER, id); }
void FrameBuffer::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
void FrameBuffer::bind_texture() { glBindTexture(GL_TEXTURE_2D, texture_id); };
void FrameBuffer::unbind_texture() { glBindTexture(GL_TEXTURE_2D, 0); };

FrameBuffer::~FrameBuffer() {
    glDeleteFramebuffers(1, &id);
    glDeleteTextures(1, &texture_id);
    if (with_depth_stencil) glDeleteRenderbuffers(1, &renderbuffer_id);
}


// example of rendering framebuffer to ImGui image

// {
//     ImGui::Begin("Scene");
//     const float win_width = ImGui::GetContentRegionAvail().x;
//     const float win_height = ImGui::GetContentRegionAvail().y;
//     framebuffer.rescale(win_width, win_height);
//     ImGui::Image((ImTextureID)framebuffer.texture_id, ImGui::GetContentRegionAvail(), ImVec2(0,1), ImVec2(1,0));
//     ImGui::End();
// }
