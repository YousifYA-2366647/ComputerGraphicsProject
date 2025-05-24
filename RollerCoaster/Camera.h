#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glm-1.0.1/glm/glm.hpp>
#include <glm-1.0.1/glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Roty;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    Camera(glm::vec3 position, glm::vec3 up, float roty, float pitchx);

    glm::mat4 GetViewMatrix();

    void ProcessKeyboard(int direction, float deltaTime);

    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean limitPitch = true);

    void ProcessMouseScroll(float yoffset);

    void updateCameraVectors();

private:
    
};

#endif // CAMERA_H
