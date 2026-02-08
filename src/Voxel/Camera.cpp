#include "Camera.h"
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/UI/MainUI.h>
#include <Voxel/UI/Panels/ViewportPanel.h>;

Camera::Camera(glm::vec3 position, float yaw, float pitch, float movementSpeed,
               float mouseSensitivity, float zoom) {
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

void Camera::ProcessInput(GLFWwindow* window) {
    Application* application = Application::GetInstance();
    if (!application)
        return;

    float velocity = movementSpeed * application->DeltaTime();

    const bool rmbDown = InputManager::IsMouseButtonDown(window, GLFW_MOUSE_BUTTON_2);
    const bool viewportHovered = MainUI::GetViewportPanel()->IsHovered();

    // Start focus ONLY if RMB pressed while hovering viewport
    if (!focused && rmbDown && viewportHovered) {
        SetFocused(true);
    }

    // Stop focus when RMB released
    if (focused && !rmbDown) {
        SetFocused(false);
    }

    if (!focused)
        return;

    // Force ImGUI mouse position to be unavailable to prevent accidental clicking on other panels
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
    if (InputManager::IsKeyDown(window, GLFW_KEY_LEFT_SHIFT))
        velocity *= 5.0f;

    if (InputManager::IsKeyDown(window, GLFW_KEY_W))
        position += front * velocity;
    if (InputManager::IsKeyDown(window, GLFW_KEY_S))
        position -= front * velocity;
    if (InputManager::IsKeyDown(window, GLFW_KEY_A))
        position -= right * velocity;
    if (InputManager::IsKeyDown(window, GLFW_KEY_D))
        position += right * velocity;
    if (InputManager::IsKeyDown(window, GLFW_KEY_Q))
        position -= up * velocity;
    if (InputManager::IsKeyDown(window, GLFW_KEY_E))
        position += up * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
    if (!focused)
        return;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (constrainPitch) {
        pitch = glm::clamp(pitch, -89.0f, 89.0f);
    }

    UpdateCameraVectors();
}

void Camera::UpdateCameraVectors() {
    // calculate the new Front vector
    glm::vec3 localFront;
    localFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    localFront.y = sin(glm::radians(pitch));
    localFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(localFront);
    // also re-calculate the Right and Up vector
    right = glm::normalize(glm::cross(
        front, worldUp)); // normalize the vectors, because their length gets closer to 0 the more
                          // you look up or down which results in slower movement.
    up = glm::normalize(glm::cross(right, front));
}

void Camera::SetFocused(bool focus) {
    if (focus == focused)
        return;

    Application* application = Application::GetInstance();
    if (!application)
        return;

    if (focus) {
        glfwSetInputMode(application->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(application->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    focused = focus;
}

void Camera::ProcessMouseScroll(float yoffset) {
    if (!MainUI::GetViewportPanel()->IsHovered()) {
        return;
    }
    zoom -= (float)yoffset;
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 45.0f)
        zoom = 45.0f;
}

float Camera::GetZoom() { return zoom; }

glm::mat4 Camera::GetViewMatrix() { return glm::lookAt(position, position + front, up); }