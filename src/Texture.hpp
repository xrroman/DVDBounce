#pragma once

#include <glad/glad.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Texture
{
public:
    Texture(const char* path)
    {
        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_2D, m_id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_set_flip_vertically_on_load(true);

        int w, h, channels;
        unsigned char* data = stbi_load(path, &w, &h, &channels, 4);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cerr << "[Texture] Failed to load '" << path << "'\n";
        }
        stbi_image_free(data);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    ~Texture()
    {
        if (m_id != 0)
            glDeleteTextures(1, &m_id);
    }

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept : m_id(other.m_id) { other.m_id = 0; }
    Texture& operator=(Texture&& other) noexcept
    {
        if (this != &other) { glDeleteTextures(1, &m_id); m_id = other.m_id; other.m_id = 0; }
        return *this;
    }

    void bind() const { glBindTexture(GL_TEXTURE_2D, m_id); }

    [[nodiscard]] GLuint getID() const { return m_id; }

private:
    GLuint m_id = 0;
};