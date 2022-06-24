#pragma once
#include "Component.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float MOVEMENTSPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class CameraComponent : public Component
{
private:
    // camera attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    // euler Angles
    float yaw;
    float pitch;
    // camera options
    float movementSpeed;
    float mouseSensitivity;
    float zoom;

public:
    // constructor with vectors
    CameraComponent(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float yaw = YAW, float pitch = PITCH,
        float movementSpeed = MOVEMENTSPEED, float mouseSensitivity = SENSITIVITY, float zoom = ZOOM);

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessInput(struct GLFWwindow* window, float deltaTime);

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset);

    float GetZoom();

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void UpdateCameraVectors();
};

