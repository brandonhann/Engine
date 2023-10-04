#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Camera.h"

class Application {
public:
    Application(int windowWidth, int windowHeight, const char* windowTitle);
    ~Application();

    void Run(); // Main loop
    void RenderBaseplate(const glm::vec3& objectColor);
    void RenderSlope(const glm::vec3& objectColor);

private:
    GLFWwindow* m_Window;
    int m_WindowWidth;
    int m_WindowHeight;
    Shader* m_Shader;
    Camera* m_Camera;

    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    void InitializeGLFW();
    void InitializeGLEW();
};