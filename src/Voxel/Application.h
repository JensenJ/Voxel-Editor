#pragma once

class Application {
  public:
    static class Application* GetInstance();

    bool Initialise();
    void Shutdown();
    bool ShouldStayOpen();
    void StartFrame();
    void EndFrame();
    float DeltaTime();
    struct GLFWwindow* GetWindow();
    class Shader* GetActiveShader();
    class FrameBuffer* GetSceneBuffer();
    int GetSceneViewportWidth();
    int GetSceneViewportHeight();
    class Camera* GetCamera();

    void SetSceneViewportWidth(int width);
    void SetSceneViewportHeight(int height);

  private:
    Application() = default;
    void InitialiseOpenGl();
    void InitialiseImGui();
    bool LoadShaders();
    void InitialiseFrameBuffer();
    void SetupCamera();

    static void UpdateFrameBufferSize(struct GLFWwindow* window, int width, int height);

  private:
    static Application* instance;
    class Camera* camera = nullptr;

    struct GLFWwindow* window = nullptr;
    class Shader* activeShaderProgram = nullptr;
    class FrameBuffer* sceneBuffer = nullptr;
    int sceneViewportWidth = 0;
    int sceneViewportHeight = 0;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    double lastTime = 0;
    int nbFrames = 0;
};