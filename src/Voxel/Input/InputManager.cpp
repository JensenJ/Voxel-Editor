#include "InputManager.h"
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/Camera.h>
#include <Voxel/EditorSettings.h>

InputManager* InputManager::instance = nullptr;

InputManager* InputManager::GetInstance() {
    if (instance == nullptr) {
        instance = new InputManager();
        // EditorSettings::OnSettingsAppliedEvent.AddObserver(
        //     [](const SettingsAppliedEvent& event) { instance->LoadBindings(); });
    }
    return instance;
}

void InputManager::BindAction(InputAction action, InputTrigger trigger,
                              std::function<void()> callback) {
    switch (trigger) {
    case InputTrigger::Pressed:
        callbacks[action].pressed.push_back(callback);
        break;
    case InputTrigger::Released:
        callbacks[action].released.push_back(callback);
        break;
    case InputTrigger::Held:
        callbacks[action].held.push_back(callback);
        break;
    }
}

bool InputManager::AddBinding(InputAction action, InputDevice device, int key, int mods) {
    KeyChord chord;
    chord.device = device;
    chord.key = key;
    chord.mods = mods;

    // Prevent conflicts
    if (lookup.find(chord) != lookup.end())
        return false;

    actionBindings[action].push_back(chord);
    RebuildLookup();
    SaveBindings();
    return true;
}

bool InputManager::RemoveBinding(InputAction action, InputDevice device, int key, int mods) {
    KeyChord chord;
    chord.device = device;
    chord.key = key;
    chord.mods = mods;

    auto it = actionBindings.find(action);
    if (it == actionBindings.end())
        return false;

    auto& vec = it->second;
    auto found = std::find(vec.begin(), vec.end(), chord);
    if (found != vec.end())
        vec.erase(found);
    else
        return false;

    RebuildLookup();
    SaveBindings();
    return true;
}

void InputManager::Update() {
    for (auto& [action, isHeld] : actionStates) {
        if (isHeld)
            TriggerAction(action, InputTrigger::Held);
    }
}

void InputManager::RebuildLookup() {
    lookup.clear();
    for (auto& [action, chords] : actionBindings) {
        for (auto& chord : chords)
            lookup[chord] = action;
    }
}

void InputManager::TriggerAction(InputAction action, InputTrigger trigger) {
    auto it = callbacks.find(action);
    if (it == callbacks.end())
        return;

    auto& actionCallbacks = it->second;

    switch (trigger) {
    case InputTrigger::Pressed:
        for (auto& func : actionCallbacks.pressed)
            func();
        break;

    case InputTrigger::Released:
        for (auto& func : actionCallbacks.released)
            func();
        break;

    case InputTrigger::Held:
        for (auto& func : actionCallbacks.held)
            func();
        break;
    }
}

// Runtime input
void InputManager::OnKey(int key, int glfwAction, int mods) {
    // Ignore repeat events — held is handled in Update()
    if (glfwAction == GLFW_REPEAT)
        return;

    KeyChord chord{};
    chord.device = InputDevice::Keyboard;
    chord.key = key;
    chord.mods = mods;

    auto it = lookup.find(chord);
    if (it == lookup.end())
        return;

    InputAction action = it->second;

    if (glfwAction == GLFW_PRESS) {
        // Prevent duplicate press if already held
        if (!actionStates[action]) {
            actionStates[action] = true;
            TriggerAction(action, InputTrigger::Pressed);
        }
    } else if (glfwAction == GLFW_RELEASE) {
        actionStates[action] = false;
        TriggerAction(action, InputTrigger::Released);
    }
}

void InputManager::OnMouseButton(int button, int action, int mods) {
    // Ignore repeat events — held is handled in Update()
    if (action == GLFW_REPEAT)
        return;

    KeyChord chord{};
    chord.device = InputDevice::MouseButton;
    chord.key = button;
    chord.mods = mods;

    auto it = lookup.find(chord);
    if (it == lookup.end())
        return;

    InputAction inputAction = it->second;

    if (action == GLFW_PRESS) {
        // Prevent duplicate press if already held
        if (!actionStates[inputAction]) {
            actionStates[inputAction] = true;
            TriggerAction(inputAction, InputTrigger::Pressed);
        }
    } else if (action == GLFW_RELEASE) {
        actionStates[inputAction] = false;
        TriggerAction(inputAction, InputTrigger::Released);
    }
}

void InputManager::SaveBindings() {}

void InputManager::LoadBindings() {}

void InputManager::RawKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods) {
    InputManager::GetInstance()->OnKey(key, action, mods);
}

void InputManager::RawMouseButtonInput(GLFWwindow* window, int button, int action, int mods) {
    InputManager::GetInstance()->OnMouseButton(button, action, mods);
}

// For mouse position and scroll, we may just forward to the camera or your existing system
void InputManager::RawMouseInput(GLFWwindow* window, double xposIn, double yposIn) {
    static bool firstMouse = true;
    static float lastX = 0.0f, lastY = 0.0f;

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go bottom -> top

    lastX = xpos;
    lastY = ypos;

    // Forward to camera if needed
    Application* app = Application::GetInstance();
    if (app) {
        Camera* cam = app->GetCamera();
        if (cam)
            cam->ProcessMouseMovement(xoffset, yoffset);
    }
}

void InputManager::RawScrollInput(GLFWwindow* window, double xoffset, double yoffset) {
    Application* app = Application::GetInstance();
    if (app) {
        Camera* cam = app->GetCamera();
        if (cam)
            cam->ProcessMouseScroll(static_cast<float>(yoffset));
    }
}