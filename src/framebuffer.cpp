#include "framebuffer.h"

#include <iostream>

FrameBuffer::FrameBuffer(int width, int height) {
    glGenFramebuffers(1, &id);
    glGenTextures(1, &texture_id);
    glGenRenderbuffers(1, &renderbuffer_id);
    bind();

    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer_id);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete !?" << std::endl;
    }

    rescale(width, height);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    unbind();
}

void FrameBuffer::rescale(int width, int height) {
    bind();
    
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);
    
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer_id);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete after resize!" << std::endl;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    unbind();
}

void FrameBuffer::bind() { glBindFramebuffer(GL_FRAMEBUFFER, id); }

void FrameBuffer::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

FrameBuffer::~FrameBuffer() {
    glDeleteFramebuffers(1, &id);
    glDeleteTextures(1, &texture_id);
    glDeleteRenderbuffers(1, &renderbuffer_id);
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
