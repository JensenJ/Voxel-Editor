#include "Application.h"
#include <Voxel/pch.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <Voxel/Camera.h>
#include <Voxel/Rendering/FrameBuffer.h>
#include <Voxel/Rendering/ShaderLoader.h>
#include <Voxel/UI/MainUI.h>

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
    MainUI::Initialise();
    if (!LoadShaders()) {
        return false;
    }
    InitialiseFrameBuffer();
    SetupCamera();
    return true;
}

void Application::Shutdown() {
    activeShaderProgram->Delete();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    delete this->sceneBuffer;
    this->sceneBuffer = nullptr;

    delete this->activeShaderProgram;
    this->activeShaderProgram = nullptr;
}

void Application::InitialiseOpenGl() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    int screenWidth = 1920;
    int screenHeight = 1080;

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Voxel Editor", NULL, NULL);
    if (window == NULL) {
        LOG_FATAL("Failed to create GLFW window");
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGL(glfwGetProcAddress)) {
        LOG_FATAL("Failed to initialise GLAD");
        glfwTerminate();
        return;
    }

    glfwSetWindowPos(window, (mode->width - screenWidth) / 2, (mode->height - screenHeight) / 2);
    glfwMaximizeWindow(window);

    int fbWidth, fbHeight;
    glfwGetFramebufferSize(window, &fbWidth, &fbHeight);
    glViewport(0, 0, fbWidth, fbHeight);
    glfwSetFramebufferSizeCallback(window, Application::UpdateFrameBufferSize);
    glfwSetCursorPosCallback(window, InputManager::RawMouseInput);
    glfwSetScrollCallback(window, InputManager::RawScrollInput);
    glfwSetKeyCallback(window, InputManager::RawKeyInput);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    this->sceneViewportWidth = fbWidth;
    this->sceneViewportHeight = fbHeight;
    this->window = window;
    LOG_INFO("Initialised OpenGL with screen size ({}, {})", fbWidth, fbHeight);
}

bool Application::LoadShaders() {
    // Load shaders
    bool shaderSuccess = false;
    std::string vertexPath =
        std::filesystem::current_path().string() + "\\resources\\shaders\\vertex.vert";
    std::string fragmentPath =
        std::filesystem::current_path().string() + "\\resources\\shaders\\fragment.frag";
    Shader shader =
        ShaderLoader::CreateShaderProgram(vertexPath.c_str(), fragmentPath.c_str(), shaderSuccess);

    // If shader compilation/linking failed
    if (!shaderSuccess) {
        LOG_FATAL("Failed to create shaders");
        glfwTerminate();
        return false;
    }
    this->activeShaderProgram = new Shader(shader);

    LOG_INFO("Loaded shaders");
    return true;
}

void Application::InitialiseFrameBuffer() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);

    int fbW, fbH;
    glfwGetFramebufferSize(window, &fbW, &fbH);

    LOG_INFO("Created framebuffer with size: ({}, {})", fbW, fbH);
    this->sceneBuffer = new FrameBuffer(fbW, fbH);
}

void Application::SetupCamera() { this->camera = new Camera(); }

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

    glClearColor(0.12f, 0.12f, 0.15f, 1.0f);
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

Camera* Application::GetCamera() { return this->camera; }

void Application::SetSceneViewportWidth(int width) { this->sceneViewportWidth = width; }

void Application::SetSceneViewportHeight(int height) { this->sceneViewportHeight = height; }

void Application::UpdateFrameBufferSize(GLFWwindow* window, int width, int height) {
    if (width == 0 || height == 0)
        return;

    glViewport(0, 0, width, height);
    if (auto* app = Application::GetInstance()) {
        app->GetSceneBuffer()->RescaleFrameBuffer(width, height);
    }
}
