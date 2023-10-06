#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:
    Camera(glm::vec3 startPosition, glm::vec3 startUp, float startYaw, float startPitch, float startMoveSpeed, float startTurnSpeed);

    void KeyControl(GLFWwindow* window, float deltaTime);
    void SetBoundingBox(float height, float width);
    bool isCollidingWithBaseplate();
    bool isCollidingWithSlope();
    bool isSlopeWalkable();
    void Jump();

    glm::mat4 CalculateViewMatrix();

    glm::vec3 GetCameraPosition() const { return position; }
    glm::vec3 GetCameraDirection() const { return glm::normalize(front); }
    glm::vec3 GetPosition() const;

    // Static methods and members for mouse control
    static void MouseControlCallback(GLFWwindow* window, double xPos, double yPos);

private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    glm::vec3 boundingBoxMin;
    glm::vec3 boundingBoxMax;

    float yaw;
    float pitch;

    float moveSpeed;
    float turnSpeed;

    float gravity;
    float verticalVelocity;
    bool isGrounded;

    void Update();

    // Static members for mouse control
    static double lastMouseX;
    static double lastMouseY;
    static bool firstMouse;
};