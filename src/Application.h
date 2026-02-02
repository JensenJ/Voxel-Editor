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

    void SetSceneViewportWidth(int width);
    void SetSceneViewportHeight(int height);

    void SetMouseLocked(bool locked);
    bool IsMouseLocked();

  private:
    Application() = default;
    void InitialiseOpenGl();
    void InitialiseImGui();
    bool LoadShaders();
    void InitialiseFrameBuffer();

    static void framebuffer_size_callback(struct GLFWwindow* window, int width, int height);
    static void mouse_callback(struct GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(struct GLFWwindow* window, double xoffset, double yoffset);
    static void key_callback(struct GLFWwindow* window, int key, int scancode, int action,
                             int mods);

  private:
    static Application* instance;

    struct GLFWwindow* window = nullptr;
    class Shader* activeShaderProgram = nullptr;
    class FrameBuffer* sceneBuffer = nullptr;
    int sceneViewportWidth = 0;
    int sceneViewportHeight = 0;
    bool mouseLocked = false;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    double lastTime = 0;
    int nbFrames = 0;

    inline static float lastMouseX = 0;
    inline static float lastMouseY = 0;
    inline static bool firstMouse = true;
};