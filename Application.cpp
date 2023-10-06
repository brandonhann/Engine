#include "Application.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Application::Application(int windowWidth, int windowHeight, const char* windowTitle)
    : m_WindowWidth(windowWidth), m_WindowHeight(windowHeight), isPaused(false), escapeKeyPressed(false) {
    InitializeGLFW();

    // Get the primary monitor's video mode
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    // Calculate desired window dimensions as a percentage of screen dimensions
    m_WindowWidth = static_cast<int>(mode->width * 0.6);
    m_WindowHeight = static_cast<int>(mode->height * 0.6);

    // Calculate the position to center the window on the screen
    int windowPosX = (mode->width - m_WindowWidth) / 2;
    int windowPosY = (mode->height - m_WindowHeight) / 2;

    // Create the window
    m_Window = glfwCreateWindow(m_WindowWidth, m_WindowHeight, windowTitle, NULL, NULL);
    if (!m_Window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }

    // Set the window position
    glfwSetWindowPos(m_Window, windowPosX, windowPosY);

    glfwMakeContextCurrent(m_Window);
    InitializeGLEW();
    m_Shader = new Shader("./basic_vertex_shader.glsl", "./basic_fragment_shader.glsl");
    m_Camera = new Camera(glm::vec3(0.0f, 1.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.5f);

    // Set the mouse callback and disable the cursor
    glfwSetCursorPosCallback(m_Window, Camera::MouseControlCallback);
    glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the window resize callback
    glfwSetFramebufferSizeCallback(m_Window, Application::FramebufferSizeCallback);
    glfwSetWindowUserPointer(m_Window, this);

    // Set the window's user pointer to the camera instance
    glfwSetWindowUserPointer(m_Window, m_Camera);
}

Application::~Application() {
    delete m_Shader;
    delete m_Camera;
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

void Application::Run() {
    while (!glfwWindowShouldClose(m_Window)) {

        if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !escapeKeyPressed) {
            isPaused = !isPaused; // Toggle pause state
            glfwSetInputMode(m_Window, GLFW_CURSOR, isPaused ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
            escapeKeyPressed = true;
        }
        else if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
            escapeKeyPressed = false;
        }

        // Compute the delta time for smooth movements
        static double lastTime = glfwGetTime();
        double currentTime = glfwGetTime();
        float deltaTime = float(currentTime - lastTime);
        lastTime = currentTime;

        // Handle camera inputs
        m_Camera->KeyControl(m_Window, deltaTime);
        

        // Clear the screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear depth buffer too

        m_Shader->Use();

        // Set light properties
        glm::vec3 lightPos(1.2f, 1.0f, 2.0f);  // Position of the light source
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f);  // White light
        glm::vec3 objectColor(0.0f, 1.0f, 0.0f);  // Color of the object

        glUniform3fv(glGetUniformLocation(m_Shader->GetProgramID(), "lightPos"), 1, &lightPos[0]);
        glUniform3fv(glGetUniformLocation(m_Shader->GetProgramID(), "viewPos"), 1, &m_Camera->GetPosition()[0]);
        glUniform3fv(glGetUniformLocation(m_Shader->GetProgramID(), "lightColor"), 1, &lightColor[0]);
        glUniform3fv(glGetUniformLocation(m_Shader->GetProgramID(), "objectColor"), 1, &objectColor[0]);

        glEnable(GL_DEPTH_TEST);


        // Render the baseplate with blue color
        RenderBaseplate(glm::vec3(0.0f, 0.0f, 1.0f));


        // Render the slope with blue color
        RenderSlope(glm::vec3(0.0f, 1.0f, 0.0f));


        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }
}

void Application::RenderSlope(const glm::vec3& objectColor) {
    // Define the vertices for the 3D slope
    float slopeVertices[] = {
        // Base
        0.0f, 0.0f, 0.0f,
        2.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 2.0f,

        // Sloped side
        0.0f, 0.0f, 0.0f,
        2.0f, 0.0f, 0.0f,
        1.0f, 2.0f, 1.0f,

        // Left side
        0.0f, 0.0f, 0.0f,
        1.0f, 2.0f, 1.0f,
        1.0f, 0.0f, 2.0f,

        // Right side
        2.0f, 0.0f, 0.0f,
        1.0f, 2.0f, 1.0f,
        1.0f, 0.0f, 2.0f
    };

    unsigned int slopeIndices[] = {
        0, 1, 2,  // Base
        3, 4, 5,  // Sloped side
        6, 7, 8,  // Left side
        9, 10, 11 // Right side
    };

    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(slopeVertices), slopeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(slopeIndices), slopeIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Render slope
    m_Shader->Use();

    // Set object color
    glUniform3fv(glGetUniformLocation(m_Shader->GetProgramID(), "objectColor"), 1, glm::value_ptr(objectColor));

    glm::mat4 view = m_Camera->CalculateViewMatrix();
    glm::mat4 projection = glm::perspective(45.0f, (float)m_WindowWidth / (float)m_WindowHeight, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(m_Shader->GetProgramID(), "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(m_Shader->GetProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, sizeof(slopeIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Application::RenderBaseplate(const glm::vec3& objectColor) {
    float vertices[] = {
        // Positions          
        -5.0f, -0.5f, -5.0f,
         5.0f, -0.5f, -5.0f,
         5.0f, -0.5f,  5.0f,
        -5.0f, -0.5f,  5.0f
    };

    unsigned int indices[] = {
        0, 1, 2,  // First Triangle
        2, 3, 0   // Second Triangle
    };

    GLuint VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Render baseplate
    m_Shader->Use();

    // Set object color
    glUniform3fv(glGetUniformLocation(m_Shader->GetProgramID(), "objectColor"), 1, glm::value_ptr(objectColor));

    glm::mat4 view = m_Camera->CalculateViewMatrix();
    glm::mat4 projection = glm::perspective(45.0f, (float)m_WindowWidth / (float)m_WindowHeight, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(m_Shader->GetProgramID(), "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(m_Shader->GetProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}


void Application::FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    app->m_WindowWidth = width;
    app->m_WindowHeight = height;
}

void Application::InitializeGLFW() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void Application::InitializeGLEW() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        exit(-1);
    }
}