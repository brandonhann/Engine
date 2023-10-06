#include "Camera.h"

// Initialize static members
double Camera::lastMouseX = 400; // Half of the default window width
double Camera::lastMouseY = 300; // Half of the default window height
bool Camera::firstMouse = true;

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, float startYaw, float startPitch, float startMoveSpeed, float startTurnSpeed)
    : front(glm::vec3(0.0f, 0.0f, -1.0f)), moveSpeed(startMoveSpeed), turnSpeed(startTurnSpeed), yaw(startYaw), pitch(startPitch) {
    position = startPosition;
    worldUp = startUp;
    gravity = -9.8f;
    verticalVelocity = 0.0f;
    isGrounded = false;
    Update();
}

glm::vec3 Camera::GetPosition() const {
    return position;
}

void Camera::KeyControl(GLFWwindow* window, float deltaTime) {
    float velocity = moveSpeed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        position += front * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position -= front * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        position -= right * velocity;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position += right * velocity;
    }

    // Apply gravity
    if (!isGrounded) {
        verticalVelocity += gravity * deltaTime;
        position.y += verticalVelocity * deltaTime;
    }

    // Check if grounded
    float groundLevel = -0.5f;  // baseplate y level
    float eyeHeight = 0.5f;    // the desired height above the baseplate

    if (isCollidingWithBaseplate() && position.y <= groundLevel + eyeHeight) {
        position.y = groundLevel + eyeHeight;
        verticalVelocity = 0.0f;
        isGrounded = true;
    }
    else {
        isGrounded = false;
    }

    // Check if colliding with the slope
    if (isCollidingWithSlope()) {
        // Calculate the height adjustment based on player's x position on the slope
        float adjustedHeight = (position.x / 2.0f) + 0.5f; // For our slope
        position.y = adjustedHeight;
    }

    // Jumping logic
    if (isGrounded && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        Jump();
    }
}

void Camera::SetBoundingBox(float height, float width) {
    boundingBoxMin = position + glm::vec3(-width / 2, 0.0f, -width / 2);
    boundingBoxMax = position + glm::vec3(width / 2, height, width / 2);
}

bool Camera::isCollidingWithBaseplate() {
    glm::vec3 baseplateMin = glm::vec3(-5.0f, -0.5f, -5.0f);
    glm::vec3 baseplateMax = glm::vec3(5.0f, 0.5f, 5.0f);

    return (position.x >= baseplateMin.x && position.x <= baseplateMax.x) &&
        (position.y >= baseplateMin.y && position.y <= baseplateMax.y + 1.5f) && // Added the eyeHeight here for y-axis collision check
        (position.z >= baseplateMin.z && position.z <= baseplateMax.z);
}

bool Camera::isCollidingWithSlope() {
    glm::vec3 slopeMin = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 slopeMax = glm::vec3(2.0f, 2.0f, 2.0f);

    bool withinBase = (position.x >= slopeMin.x && position.x <= slopeMax.x) &&
        (position.z >= slopeMin.z && position.z <= slopeMax.z);

    // Check if player is above the slope's surface
    bool aboveSlopeSurface = position.y <= position.x;

    return withinBase && aboveSlopeSurface;
}

bool Camera::isSlopeWalkable() {
    float inclineAngle = glm::degrees(atan2(2.0f, 1.0f)); // Height over half the base
    return inclineAngle < 45.0f; // Assuming walkable incline is less than 45 degrees
}

void Camera::Jump() {
    if (isGrounded) {
        verticalVelocity = 5.0f;
        isGrounded = false;
    }
}

glm::mat4 Camera::CalculateViewMatrix() {
    return glm::lookAt(position, position + front, up);
}

void Camera::Update() {
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

void Camera::MouseControlCallback(GLFWwindow* window, double xPos, double yPos) {
    Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));

    if (camera->firstMouse) {
        camera->lastMouseX = xPos;
        camera->lastMouseY = yPos;
        camera->firstMouse = false;
    }

    double xOffset = xPos - camera->lastMouseX;
    double yOffset = camera->lastMouseY - yPos; // Reversed since y-coordinates go from bottom to top

    camera->lastMouseX = xPos;
    camera->lastMouseY = yPos;

    float sensitivity = 0.1f; // Adjust this value as needed
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    camera->yaw += xOffset;
    camera->pitch += yOffset;

    // To prevent the screen from getting flipped when the pitch is out of bounds
    if (camera->pitch > 89.0f) camera->pitch = 89.0f;
    if (camera->pitch < -89.0f) camera->pitch = -89.0f;

    camera->Update();
}