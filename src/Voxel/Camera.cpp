#include "Camera.h"
#include <Voxel/pch.h>
#include <Voxel/UI/MainUI.h>

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
    if (application == nullptr) {
        return;
    }
    float velocity = movementSpeed * application->DeltaTime();

    if (InputManager::IsMouseButtonDown(window, GLFW_MOUSE_BUTTON_2)) {
        SetFocused(true);
        if (InputManager::IsKeyDown(window, GLFW_KEY_LEFT_SHIFT))
            velocity = velocity * 5;
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
    } else {
        SetFocused(false);
    }
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {
    if (!IsFocused()) {
        return;
    }
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch) {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }
    // update Front, Right and Up Vectors using the updated Euler angles
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
    if (focus == lastFocused)
        return;
    Application* application = Application::GetInstance();
    if (!application)
        return;
    ImGuiIO& io = ImGui::GetIO();
    if (focus) {
        // Only allow focus if hovering over the scene viewport
        if (!MainUI::IsSceneViewportHovered())
            return;

        glfwSetInputMode(application->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        io.ConfigFlags |= ImGuiConfigFlags_NoMouse | ImGuiConfigFlags_NoKeyboard;
    } else {
        glfwSetInputMode(application->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        io.ConfigFlags &= ~(ImGuiConfigFlags_NoMouse | ImGuiConfigFlags_NoKeyboard);
    }
    this->focused = focus;
    this->lastFocused = focus;
}

void Camera::ProcessMouseScroll(float yoffset) {
    if (!MainUI::IsSceneViewportHovered()) {
        return;
    }
    zoom -= (float)yoffset;
    if (zoom < 1.0f)
        zoom = 1.0f;
    if (zoom > 45.0f)
        zoom = 45.0f;
}

float Camera::GetZoom() { return zoom; }

bool Camera::IsFocused() {
    Application* application = Application::GetInstance();
    if (application == nullptr) {
        return false;
    }
    int mouseLock = glfwGetInputMode(application->GetWindow(), GLFW_CURSOR);
    return mouseLock == GLFW_CURSOR_DISABLED;
}

glm::mat4 Camera::GetViewMatrix() { return glm::lookAt(position, position + front, up); }