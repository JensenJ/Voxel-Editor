#pragma once
#include <Voxel/pch.h>

// Default camera values
const float YAW = -45.0f;
const float PITCH = -30.0f;
const float MOVEMENTSPEED = 10.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
const glm::vec3 POSITION = glm::vec3(-4.1f, 4.6f, 5.0f);

class Camera {
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
    bool focused;

  public:
    // constructor with vectors
    Camera(glm::vec3 position = POSITION, float yaw = YAW, float pitch = PITCH,
           float movementSpeed = MOVEMENTSPEED, float mouseSensitivity = SENSITIVITY,
           float zoom = ZOOM);

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();

    void ProcessInput(struct GLFWwindow* window);

    // processes input received from a mouse input system. Expects the offset value in both the x
    // and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical
    // wheel-axis
    void ProcessMouseScroll(float yoffset);

    float GetZoom();

  private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void UpdateCameraVectors();
    void SetFocused(bool focus);
};
