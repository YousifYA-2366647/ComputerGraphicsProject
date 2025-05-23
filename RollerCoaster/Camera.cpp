#include "Camera.h"


Camera::Camera(glm::vec3 position, glm::vec3 up, float roty, float pitchx)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
    MovementSpeed(5.0f),
    MouseSensitivity(0.1f),
    Zoom(50.0f) 
{
    Position = position;
    WorldUp = up;
    Roty = roty;
    Pitch = pitchx;
    updateCameraVectors();
}


glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(int direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    if (direction == GLFW_KEY_W)
        Position += Front * velocity;
    if (direction == GLFW_KEY_S)
        Position -= Front * velocity;
    if (direction == GLFW_KEY_A)
        Position -= Right * velocity;
    if (direction == GLFW_KEY_D)
        Position += Right * velocity;
    if (direction == GLFW_KEY_Q)
        Position -= Up * velocity;
    if (direction == GLFW_KEY_E)
        Position += Up * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean limitPitchx) {
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Roty += xoffset;
    Pitch += yoffset;

    if (limitPitchx) {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(Roty)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Roty)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);

    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}

void Camera::ProcessMouseScroll(float yoffset) {
    Zoom -= yoffset;
    if (Zoom < 10.0f)
        Zoom = 10.0f;
    if (Zoom > 90.0f)
        Zoom = 90.0f;
}

