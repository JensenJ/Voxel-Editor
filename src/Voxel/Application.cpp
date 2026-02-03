#include "Application.h"
#include <Voxel/pch.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <Voxel/Camera.h>
#include <Voxel/Entity/Entity.h>
#include <Voxel/Entity/EntityRegistry.h>
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
    InitialiseImGui();
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
    int screenWidth = mode->width;
    int screenHeight = mode->height;

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

    glViewport(0, 0, screenWidth, screenHeight);
    glfwSetFramebufferSizeCallback(window, Application::UpdateFrameBufferSize);
    glfwSetCursorPosCallback(window, InputManager::RawMouseInput);
    glfwSetScrollCallback(window, InputManager::RawScrollInput);
    glfwSetKeyCallback(window, InputManager::RawKeyInput);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    this->sceneViewportWidth = screenWidth;
    this->sceneViewportHeight = screenHeight;
    this->window = window;
    LOG_TRACE("Initialised OpenGL");
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
    LOG_TRACE("Initialised ImGui");
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

    LOG_TRACE("Loaded shaders");
    return true;
}

void Application::InitialiseFrameBuffer() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);

    double lastTime = glfwGetTime();

    this->sceneBuffer = new FrameBuffer(sceneViewportWidth, sceneViewportHeight);
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

Camera* Application::GetCamera() { return this->camera; }

void Application::SetSceneViewportWidth(int width) { this->sceneViewportWidth = width; }

void Application::SetSceneViewportHeight(int height) { this->sceneViewportHeight = height; }

void Application::SetMouseLocked(bool lock) {
    if (lock) {
        glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

bool Application::IsMouseLocked() {
    int mouseLock = glfwGetInputMode(this->window, GLFW_CURSOR);
    return mouseLock == GLFW_CURSOR_DISABLED;
}

void Application::UpdateFrameBufferSize(GLFWwindow* window, int width, int height) {
    Application* application = Application::GetInstance();
    if (application == nullptr) {
        return;
    }

    glViewport(0, 0, width, height);
    application->GetSceneBuffer()->RescaleFrameBuffer(width, height);
}
