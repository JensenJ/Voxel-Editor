#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/Camera.h>
#include <Voxel/ECS/Components/MeshComponent.h>
#include <Voxel/ECS/Components/MetaComponent.h>
#include <Voxel/ECS/Components/TransformComponent.h>
#include <Voxel/ECS/Systems/RenderSystem.h>
#include <Voxel/ECS/Systems/TransformSystem.h>
#include <Voxel/ECS/Systems/VisibilitySystem.h>
#include <Voxel/Rendering/RawModel.h>
#include <Voxel/Rendering/ShaderLoader.h>

bool mouseLocked = true;
bool wireframeMode = false;

void ToggleWireframeMode();
void CloseWindow();

int main() {
    Log::Init();
    Application* application = Application::GetInstance();
    if (!application->Initialise()) {
        return -1;
    }

    Vertex vertexArray[] = {
        // Positions							//Colours
        {glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 0.0f)},   // Front face top right
        {glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f)},  // Front face bottom right
        {glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f)}, // Front face bottom left
        {glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f)},  // Front face top left

        {glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 0.0f)},   // Back face top right
        {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 1.0f)},  // Back face bottom right
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 1.0f)}, // Back face bottom left
        {glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 1.0f)}   // Back face top left
    };

    unsigned int indices[] = {
        3, 1, 0, // first triangle - front top right, front bottom right, front top left
        3, 2, 1, // second triangle - front bottom right, front bottom left, front top left
        4, 5, 7, // first triangle - back top right, back bottom right, back top left
        5, 6, 7, // second triangle - back bottom right, back bottom left, back top left
        0, 5, 4, // first triangle - front top right, back bottom right, back top right
        1, 5, 0, // second triangle - front bottom right, back bottom right, front top right
        7, 6, 3, // first triangle - front top left, back bottom left, back top left
        3, 6, 2, // second triangle - front bottom left, back bottom left, front top left
        7, 3, 4, // first triangle - back top left, front top left, back top right
        3, 0, 4, // second triangle - back top right, front top right, front top left
        5, 2, 6, // first triangle - back bottom left, front bottom left, back bottom right
        5, 1, 2, // second triangle - back bottom right, front bottom right, front bottom left
    };

    std::vector<Vertex> verticesVector(std::begin(vertexArray), std::end(vertexArray));
    std::vector<unsigned int> indicesVector(std::begin(indices), std::end(indices));

    RawModel testModel = RawModel(verticesVector, indicesVector);
    Camera* camera = application->GetCamera();

    EntityRegistry* entityRegistry = EntityRegistry::GetInstance();
    if (entityRegistry == nullptr) {
        LOG_FATAL("Failed to get entity registry");
        return -2;
    }

    RenderSystem::Init(application, camera, entityRegistry);
    TransformSystem::Init(entityRegistry);
    VisibilitySystem::Init(entityRegistry);

    InputManager* inputManager = InputManager::GetInstance();
    if (inputManager == nullptr) {
        LOG_FATAL("Failed to get input manager");
        return -3;
    }

    inputManager->BindNewKey(GLFW_KEY_F1, GLFW_PRESS, 0, ToggleWireframeMode);
    inputManager->BindNewKey(GLFW_KEY_ESCAPE, GLFW_PRESS, 0, CloseWindow);

    // Create entities
    for (int x = 0; x < 2; x++) {
        for (int y = 0; y < 2; y++) {
            for (int z = 0; z < 2; z++) {
                Entity entity = entityRegistry->CreateEntity();
                auto& meta = entityRegistry->AddComponent<MetaComponent>(
                    entity, std::format("Cube ({}, {}, {})", x, y, z), true);
                auto& transform =
                    entityRegistry->AddComponent<TransformComponent>(entity, glm::vec3(x, y, z));
                auto& mesh = entityRegistry->AddComponent<MeshComponent>(entity, &testModel);
                auto& hierarchy = entityRegistry->AddComponent<HierarchyComponent>(entity);
            }
        }
    }

    while (application->ShouldStayOpen()) {
        application->StartFrame();

        // Draw wireframe if enabled
        if (wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        Camera* camera = application->GetCamera();
        if (camera == nullptr) {
            glfwTerminate();
            LOG_FATAL("Failed to get camera");
            return -4;
        };
        camera->ProcessInput(application->GetWindow());
        VisibilitySystem::Run();
        TransformSystem::Run();
        RenderSystem::Run();

        application->EndFrame();
    }

    inputManager->Cleanup();
    delete inputManager;
    inputManager = nullptr;
    testModel.DeleteModel();
    entityRegistry->Cleanup();
    delete entityRegistry;
    entityRegistry = nullptr;
    application->Shutdown();
    delete application;
    application = nullptr;
    LOG_INFO("Exiting...");

    return 0;
}

void ToggleWireframeMode() { wireframeMode = !wireframeMode; }

void CloseWindow() {
    Application* application = Application::GetInstance();
    if (application == nullptr) {
        return;
    }
    glfwSetWindowShouldClose(application->GetWindow(), true);
}