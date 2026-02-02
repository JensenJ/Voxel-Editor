#include "Application.h"
#include <filesystem>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "Entity/Components/CameraComponent.h"
#include "Entity/Entity.h"
#include "Entity/EntityRegistry.h"
#include "InputManager.h"
#include "Rendering/FrameBuffer.h"
#include "Rendering/ShaderLoader.h"
#include "UI/MainUI.h"

Application* Application::instance = nullptr;

Application* Application::GetInstance() {
    if (instance == nullptr) {
        instance = new Application();
    }
    return instance;
}

bool Application::Initialise() {
    InitialiseOpenGl();
    if (window == nullptr) {
        return false;
    }
    InitialiseImGui();
    if (!LoadShaders()) {
        return false;
    }
    InitialiseFrameBuffer();
    return true;
}

void Application::Shutdown() {
    activeShaderProgram->Delete();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void Application::InitialiseOpenGl() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    int screenWidth = mode->width;
    int screenHeight = mode->height;

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Voxel Editor", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cout << "Failed to initialise GLAD" << std::endl;
        glfwTerminate();
        return;
    }

    glfwSetWindowPos(window, (mode->width - screenWidth) / 2, (mode->height - screenHeight) / 2);
    glfwMaximizeWindow(window);

    glViewport(0, 0, screenWidth, screenHeight);
    glfwSetFramebufferSizeCallback(window, Application::framebuffer_size_callback);
    glfwSetCursorPosCallback(window, Application::mouse_callback);
    glfwSetScrollCallback(window, Application::scroll_callback);
    glfwSetKeyCallback(window, Application::key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    this->sceneViewportWidth = screenWidth;
    this->sceneViewportHeight = screenHeight;
    this->window = window;
}

void Application::InitialiseImGui() {
    // TODO: Load from settings, if it doesn't exist, try get from OS scale
    float uiScale = 1.25f;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    std::string fontPath =
        std::filesystem::current_path().string() + "\\resources\\fonts\\Roboto-Regular.ttf";
    io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 18.0f * uiScale);
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows

    ImGui::StyleColorsDark();
    ImGui::GetStyle().ScaleAllSizes(uiScale);

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look
    // identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");
}

bool Application::LoadShaders() {
    // Load shaders
    bool shaderSuccess = false;
    std::string vertexPath =
        std::filesystem::current_path().string() + "\\resources\\shaders\\vertex.vert";
    std::string fragmentPath =
        std::filesystem::current_path().string() + "\\resources\\shaders\\fragment.frag";
    Shader shaderProgram =
        ShaderLoader::CreateShaderProgram(vertexPath.c_str(), fragmentPath.c_str(), shaderSuccess);

    // If shader compilation/linking failed
    if (!shaderSuccess) {
        std::cout << "Failed to create shaders" << std::endl;
        glfwTerminate();
        return false;
    }
    std::cout << "Created Shaders" << std::endl;
    this->activeShaderProgram = &shaderProgram;
    return true;
}

void Application::InitialiseFrameBuffer() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);

    double lastTime = glfwGetTime();

    this->sceneBuffer = new FrameBuffer(sceneViewportWidth, sceneViewportHeight);
}

bool Application::ShouldStayOpen() { return !glfwWindowShouldClose(window); }

void Application::StartFrame() {
    // Calculate delta time
    float currentFrame = static_cast<float>(glfwGetTime());
    this->deltaTime = currentFrame - this->lastFrame;
    this->lastFrame = currentFrame;

    // Calculate FPS
    double currentTime = glfwGetTime();
    this->nbFrames++;
    if (currentTime - lastTime >= 1.0) {
        std::string title = "Voxel Editor [";
        title += std::to_string(this->nbFrames);
        title += " FPS, ";
        title += std::to_string(1000.0 / double(this->nbFrames));
        title += " ms]";
        glfwSetWindowTitle(this->window, title.c_str());

        this->nbFrames = 0;
        this->lastTime += 1.0;
    }

    this->activeShaderProgram->Use();

    this->sceneBuffer->Bind();
    glEnable(GL_DEPTH_TEST);

    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Application::EndFrame() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    sceneBuffer->Unbind();
    glDisable(GL_DEPTH_TEST); // Disable depth test so screen-space quad isnt discarded

    // Render over previous frame
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Render ImGui stuff
    MainUI::RenderUI();

    glfwSwapBuffers(window);
}

float Application::DeltaTime() { return this->deltaTime; }

GLFWwindow* Application::GetWindow() { return this->window; }

Shader* Application::GetActiveShader() { return this->activeShaderProgram; }

FrameBuffer* Application::GetSceneBuffer() { return this->sceneBuffer; }

int Application::GetSceneViewportWidth() { return this->sceneViewportWidth; }

int Application::GetSceneViewportHeight() { return this->sceneViewportHeight; }

void Application::SetSceneViewportWidth(int width) { this->sceneViewportWidth = width; }

void Application::SetSceneViewportHeight(int height) { this->sceneViewportHeight = height; }

void Application::SetMouseLocked(bool locked) {
    if (locked) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        this->mouseLocked = false;
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        this->mouseLocked = true;
    }
}

bool Application::IsMouseLocked() { return this->mouseLocked; }

void Application::mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    Application* application = Application::GetInstance();
    if (application == nullptr) {
        return;
    }

    EntityRegistry* entityRegistry = EntityRegistry::GetInstance();
    if (entityRegistry == nullptr) {
        return;
    }

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastMouseX;
    float yoffset = lastMouseY - ypos; // reversed since y-coordinates go from bottom to top

    lastMouseX = xpos;
    lastMouseY = ypos;

    // Only process the mouse moving if the mouse is locked to the viewport
    if (application->IsMouseLocked() == true) {
        CameraComponent* camComp = entityRegistry->camera->GetComponent<CameraComponent>();
        if (camComp == nullptr) {
            return;
        }
        camComp->ProcessMouseMovement(xoffset, yoffset);
    }
}

void Application::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    InputManager* manager = InputManager::GetInstance();
    if (manager == nullptr) {
        return;
    }

    manager->KeyCallback(window, key, scancode, action, mods);
}

void Application::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    EntityRegistry* entityRegistry = EntityRegistry::GetInstance();
    if (entityRegistry == nullptr) {
        return;
    }

    CameraComponent* camComp = entityRegistry->camera->GetComponent<CameraComponent>();
    if (camComp == nullptr) {
        return;
    }
    camComp->ProcessMouseScroll(static_cast<float>(yoffset));
}

void Application::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    Application* application = Application::GetInstance();
    if (application == nullptr) {
        return;
    }

    glViewport(0, 0, width, height);
    application->GetSceneBuffer()->RescaleFrameBuffer(width, height);
}
