#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <random>

#include "Shader.hpp"
#include "Texture.hpp"

struct Quad
{
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;

    Quad(float size)
    {
        const float v[] = {
             size,  size, 0.0f,  1.0f, 1.0f,
             size, -size, 0.0f,  1.0f, 0.0f,
            -size, -size, 0.0f,  0.0f, 0.0f,
            -size,  size, 0.0f,  0.0f, 1.0f,
        };
        const unsigned int idx[] = { 0, 1, 3, 1, 2, 3 };

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(v), v, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
            reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
            reinterpret_cast<void*>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    ~Quad()
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }

    Quad(const Quad&) = delete;
    Quad& operator=(const Quad&) = delete;

    void draw() const
    {
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
};

struct Logo
{
    float px = 0.0f;
    float py = 0.0f;
    float vx = 0.6f;
    float vy = 0.4f;
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
};

static void randomColor(Logo& logo, std::mt19937& rng)
{
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    logo.r = dist(rng);
    logo.g = dist(rng);
    logo.b = dist(rng);
}

static void update(Logo& logo, float wall, float dt, std::mt19937& rng)
{
    logo.px += logo.vx * dt;
    logo.py += logo.vy * dt;

    if (logo.px > wall) { logo.px = wall; logo.vx = -logo.vx; randomColor(logo, rng); }
    if (logo.px < -wall) { logo.px = -wall; logo.vx = -logo.vx; randomColor(logo, rng); }
    if (logo.py > wall) { logo.py = wall; logo.vy = -logo.vy; randomColor(logo, rng); }
    if (logo.py < -wall) { logo.py = -wall; logo.vy = -logo.vy; randomColor(logo, rng); }
}

static void onFramebufferResize(GLFWwindow*, int w, int h)
{
    glViewport(0, 0, w, h);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(900, 600, "DVD Bounce", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, onFramebufferResize);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "[App] Failed to initialise GLAD\n";
        glfwTerminate();
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader  shader("shaders/vertex.glsl", "shaders/fragment.glsl");
    Texture dvdLogo("assets/textures/DVD_logo.png");

    static constexpr float kSize = 0.15f;
    Quad quad(kSize);

    Logo logo;
    const float wall = 1.0f - kSize;

    std::mt19937 rng(std::random_device{}());
    randomColor(logo, rng);

    float lastTime = static_cast<float>(glfwGetTime());

    while (!glfwWindowShouldClose(window))
    {
        const float now = static_cast<float>(glfwGetTime());
        const float dt = now - lastTime;
        lastTime = now;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        update(logo, wall, dt, rng);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        shader.setVec2("offset", logo.px, logo.py);
        shader.setVec3("bounceColor", logo.r, logo.g, logo.b);
        dvdLogo.bind();
        quad.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}