#include "CameraComponent.h"
#include <iostream>
#include <GLFW\glfw3.h>

CameraComponent::CameraComponent(glm::vec3 position, float yaw, float pitch, float movementSpeed, float mouseSensitivity, float zoom)
{
	this->up = glm::vec3(0.0f, 1.0f, 0.0f);
	this->front = glm::vec3(0.0f, 0.0f, -1.0f);
	this->position = position;
	this->worldUp = up;
	this->yaw = yaw;
	this->pitch = pitch;
    this->movementSpeed = movementSpeed;
    this->mouseSensitivity = mouseSensitivity;
    this->zoom = zoom;
	UpdateCameraVectors();
}

void CameraComponent::ProcessInput(GLFWwindow* window, float deltaTime)
{
    float velocity = movementSpeed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        velocity = velocity * 5;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += front * velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= front * velocity;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= right * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += right * velocity;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        position -= up * velocity;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        position += up * velocity;
}

void CameraComponent::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }
    // update Front, Right and Up Vectors using the updated Euler angles
    UpdateCameraVectors();
}

void CameraComponent::UpdateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 localFront;
    localFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    localFront.y = sin(glm::radians(pitch));
    localFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(localFront);
    // also re-calculate the Right and Up vector
    right = glm::normalize(glm::cross(front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    up = glm::normalize(glm::cross(right, front));
}

void CameraComponent::ProcessMouseScroll(float yoffset)
{
    zoom -= (float)yoffset;
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 45.0f)
        zoom = 45.0f;
}

float CameraComponent::GetZoom()
{
    return zoom;
}

glm::mat4 CameraComponent::GetViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}