#include "Camera.h"
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/UI/MainUI.h>
#include <Voxel/UI/Panels/ViewportPanel.h>

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

    InputManager* inputManager = InputManager::GetInstance();

    // Create action bindings
    inputManager->BindAction(InputAction::FreeCam_FocusViewport, InputTrigger::Pressed,
                             std::bind(&Camera::SetFocused, this, true));
    inputManager->BindAction(InputAction::FreeCam_FocusViewport, InputTrigger::Released,
                             std::bind(&Camera::SetFocused, this, false));
    inputManager->BindAction(InputAction::FreeCam_FocusViewport, InputTrigger::Held,
                             std::bind(&Camera::WhileFocused, this));

    inputManager->BindAction(InputAction::FreeCam_MoveForward, InputTrigger::Held,
                             std::bind(&Camera::MoveForward, this));
    inputManager->BindAction(InputAction::FreeCam_MoveBackward, InputTrigger::Held,
                             std::bind(&Camera::MoveBackward, this));
    inputManager->BindAction(InputAction::FreeCam_MoveLeft, InputTrigger::Held,
                             std::bind(&Camera::MoveLeft, this));
    inputManager->BindAction(InputAction::FreeCam_MoveRight, InputTrigger::Held,
                             std::bind(&Camera::MoveRight, this));
    inputManager->BindAction(InputAction::FreeCam_MoveUp, InputTrigger::Held,
                             std::bind(&Camera::MoveUp, this));
    inputManager->BindAction(InputAction::FreeCam_MoveDown, InputTrigger::Held,
                             std::bind(&Camera::MoveDown, this));

    inputManager->BindAction(InputAction::FreeCam_ZoomIn, InputTrigger::Pressed,
                             std::bind(&Camera::ZoomIn, this));
    inputManager->BindAction(InputAction::FreeCam_ZoomOut, InputTrigger::Pressed,
                             std::bind(&Camera::ZoomOut, this));

    inputManager->BindAction(InputAction::FreeCam_IncreaseSpeed, InputTrigger::Pressed,
                             std::bind(&Camera::IncreaseSpeed, this));
    inputManager->BindAction(InputAction::FreeCam_DecreaseSpeed, InputTrigger::Pressed,
                             std::bind(&Camera::DecreaseSpeed, this));

    // TODO: TEMPORARY (until configurable keybinds are implemented) - Create key bindings
    inputManager->AddBinding(InputAction::FreeCam_FocusViewport, InputDevice::MouseButton,
                             GLFW_MOUSE_BUTTON_2, 0);
    inputManager->AddBinding(InputAction::FreeCam_MoveForward, InputDevice::Keyboard, GLFW_KEY_W,
                             0);
    inputManager->AddBinding(InputAction::FreeCam_MoveBackward, InputDevice::Keyboard, GLFW_KEY_S,
                             0);
    inputManager->AddBinding(InputAction::FreeCam_MoveLeft, InputDevice::Keyboard, GLFW_KEY_A, 0);
    inputManager->AddBinding(InputAction::FreeCam_MoveRight, InputDevice::Keyboard, GLFW_KEY_D, 0);
    inputManager->AddBinding(InputAction::FreeCam_MoveUp, InputDevice::Keyboard, GLFW_KEY_SPACE, 0);
    inputManager->AddBinding(InputAction::FreeCam_MoveDown, InputDevice::Keyboard,
                             GLFW_KEY_LEFT_SHIFT, 0);

    inputManager->AddBinding(InputAction::FreeCam_ZoomIn, InputDevice::MouseScroll, SCROLL_UP,
                             GLFW_MOD_CONTROL);
    inputManager->AddBinding(InputAction::FreeCam_ZoomOut, InputDevice::MouseScroll, SCROLL_DOWN,
                             GLFW_MOD_CONTROL);

    inputManager->AddBinding(InputAction::FreeCam_IncreaseSpeed, InputDevice::MouseScroll,
                             SCROLL_UP, 0);
    inputManager->AddBinding(InputAction::FreeCam_DecreaseSpeed, InputDevice::MouseScroll,
                             SCROLL_DOWN, 0);

    UpdateCameraVectors();
}

void Camera::ProcessMouseMovement(float xposIn, float yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (!mouseInitialized) {
        lastX = xpos;
        lastY = ypos;
        mouseInitialized = true;
        return;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    if (!focused)
        return;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    UpdateCameraVectors();
}

void Camera::UpdateCameraVectors() {
    // calculate the new Front vector
    glm::vec3 localFront;
    localFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    localFront.y = sin(glm::radians(pitch));
    localFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(localFront);
    // also re-calculate the right vector
    right = glm::normalize(glm::cross(front, worldUp));
    // up = glm::normalize(glm::cross(right, front)); Keep up and down global, TODO: this may be
    // worth making a setting
}

void Camera::SetFocused(bool focus) {
    if (focus == focused)
        return;

    bool viewportHovered = MainUI::GetViewportPanel()->IsHovered();
    if (focus && !viewportHovered)
        return;

    Application* application = Application::GetInstance();
    if (!application)
        return;

    if (focus) {
        glfwSetInputMode(application->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        mouseInitialized = false;
    } else {
        glfwSetInputMode(application->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    focused = focus;
}

float Camera::GetVelocity() {
    Application* application = Application::GetInstance();
    if (!application)
        return movementSpeed;
    return movementSpeed * application->DeltaTime();
}

void Camera::WhileFocused() {
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
}

void Camera::MoveForward() {
    if (!focused)
        return;
    position += front * GetVelocity();
}

void Camera::MoveBackward() {
    if (!focused)
        return;
    position -= front * GetVelocity();
}

void Camera::MoveLeft() {
    if (!focused)
        return;
    position -= right * GetVelocity();
}

void Camera::MoveRight() {
    if (!focused)
        return;
    position += right * GetVelocity();
}

void Camera::MoveUp() {
    if (!focused)
        return;
    position += up * GetVelocity();
}

void Camera::MoveDown() {
    if (!focused)
        return;
    position -= up * GetVelocity();
}

void Camera::ZoomIn() {
    if (!MainUI::GetViewportPanel()->IsHovered() || focused)
        return;
    float factor = std::pow(1.1, (float)-1);
    zoom = std::clamp(zoom * factor, 1.0f, 45.0f);
}

void Camera::ZoomOut() {
    if (!MainUI::GetViewportPanel()->IsHovered() || focused)
        return;
    float factor = std::pow(1.1, (float)1);
    zoom = std::clamp(zoom * factor, 1.0f, 45.0f);
}

void Camera::IncreaseSpeed() {
    if (!focused)
        return;
    float factor = std::pow(1.1, (float)1);
    movementSpeed = std::clamp(movementSpeed * factor, 1.0f, 200.0f);
}

void Camera::DecreaseSpeed() {
    if (!focused)
        return;
    float factor = std::pow(1.1, (float)-1);
    movementSpeed = std::clamp(movementSpeed * factor, 1.0f, 200.0f);
}

float Camera::GetZoom() { return zoom; }

glm::mat4 Camera::GetViewMatrix() { return glm::lookAt(position, position + front, up); }