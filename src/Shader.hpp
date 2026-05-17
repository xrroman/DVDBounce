#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    Shader(const char* vertexPath, const char* fragmentPath)
    {
        const std::string vertSource = loadFile(vertexPath);
        const std::string fragSource = loadFile(fragmentPath);

        const GLuint vert = compile(GL_VERTEX_SHADER, vertSource);
        const GLuint frag = compile(GL_FRAGMENT_SHADER, fragSource);

        m_id = glCreateProgram();
        glAttachShader(m_id, vert);
        glAttachShader(m_id, frag);
        glLinkProgram(m_id);

        GLint success = 0;
        glGetProgramiv(m_id, GL_LINK_STATUS, &success);
        if (!success)
        {
            char log[512];
            glGetProgramInfoLog(m_id, sizeof(log), nullptr, log);
            std::cerr << "[Shader] Link error:\n" << log << '\n';
        }

        glDeleteShader(vert);
        glDeleteShader(frag);
    }

    ~Shader()
    {
        if (m_id != 0)
            glDeleteProgram(m_id);
    }

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept : m_id(other.m_id) { other.m_id = 0; }
    Shader& operator=(Shader&& other) noexcept
    {
        if (this != &other) { glDeleteProgram(m_id); m_id = other.m_id; other.m_id = 0; }
        return *this;
    }

    void use() const { glUseProgram(m_id); }

    void setVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(m_id, name.c_str()), x, y);
    }

    void setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(m_id, name.c_str()), x, y, z);
    }

private:
    GLuint m_id = 0;

    static std::string loadFile(const char* path)
    {
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            file.open(path);
            std::ostringstream ss;
            ss << file.rdbuf();
            return ss.str();
        }
        catch (const std::ifstream::failure& e)
        {
            std::cerr << "[Shader] Could not read '" << path << "': " << e.what() << '\n';
            return {};
        }
    }

    static GLuint compile(GLenum type, const std::string& source)
    {
        const GLuint id = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);

        GLint success = 0;
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char log[512];
            glGetShaderInfoLog(id, sizeof(log), nullptr, log);
            const char* typeName = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
            std::cerr << "[Shader] " << typeName << " compile error:\n" << log << '\n';
        }
        return id;
    }
};