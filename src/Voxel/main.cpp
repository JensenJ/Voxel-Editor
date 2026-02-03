#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#pragma once

#include <Voxel/pch.h>
#include <glm/gtc/type_ptr.hpp>
#include <Voxel/Camera.h>
#include <Voxel/Entity/Components/Component.h>
#include <Voxel/Entity/Components/MeshRendererComponent.h>
#include <Voxel/Entity/Components/TransformComponent.h>
#include <Voxel/Entity/Entity.h>
#include <Voxel/Rendering/EntityRenderer.h>
#include <Voxel/Rendering/FrameBuffer.h>
#include <Voxel/Rendering/RawModel.h>
#include <Voxel/Rendering/ShaderLoader.h>

bool mouseLocked = true;
bool wireframeMode = false;

void ToggleMouseCursor();
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
    EntityRenderer renderer = EntityRenderer();

    EntityRegistry* entityRegistry = EntityRegistry::GetInstance();
    if (entityRegistry == nullptr) {
        return -2;
    }

    // Basic Input binding
    InputManager* inputManager = InputManager::GetInstance();
    if (inputManager == nullptr) {
        return -3;
    }

    inputManager->BindNewKey(GLFW_KEY_9, GLFW_PRESS, 0, ToggleWireframeMode);
    inputManager->BindNewKey(GLFW_KEY_ESCAPE, GLFW_PRESS, 0, CloseWindow);

    // Create entities
    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 2; y++) {
            for (int z = 0; z < 10; z++) {
                Entity* entity = entityRegistry->CreateEntity("Cube (" + std::to_string(x) + ", " +
                                                              std::to_string(y) + ", " +
                                                              std::to_string(z) + ")");
                entity->AddComponent<TransformComponent>(glm::vec3(x, y, z));
                entity->AddComponent<MeshRendererComponent>(&testModel);
            }
        }
    }

    std::vector<TransformComponent*> allTransforms =
        entityRegistry->GetAllComponentsOfType<TransformComponent>();
    std::vector<MeshRendererComponent*> allMeshRenderers =
        entityRegistry->GetAllComponentsOfType<MeshRendererComponent>();

    while (application->ShouldStayOpen()) {
        application->StartFrame();

        // Draw wireframe if enabled
        if (wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        Camera* camera = application->GetCamera();
        if (camera == nullptr) {
            glfwTerminate();
            LOG_ERROR("Failed to get camera");
            return -3;
        };

        // Update all components in the entity registry
        entityRegistry->UpdateAllComponents(application->DeltaTime());
        camera->ProcessInput(application->GetWindow());

        glm::mat4 view = camera->GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera->GetZoom()),
                                                (float)application->GetSceneViewportWidth() /
                                                    (float)application->GetSceneViewportHeight(),
                                                0.1f, 100.0f);

        application->GetActiveShader()->SetMat4("view", view);
        application->GetActiveShader()->SetMat4("projection", projection);

        // Move selected entities up
        for (auto iter = entityRegistry->selectedEntities.begin();
             iter != entityRegistry->selectedEntities.end(); ++iter) {
            TransformComponent* transform = (*iter)->GetComponent<TransformComponent>();
            if (transform != nullptr) {
                transform->AddPosition(glm::vec3(0.0f, 0.25f, 0.0f) * application->DeltaTime());
            }
        }

        for (unsigned int i = 0; i < allMeshRenderers.size(); i++) {
            Entity* entity =
                entityRegistry->GetEntityFromID(allMeshRenderers[i]->GetOwningEntityID());
            if (entity == nullptr) {
                LOG_ERROR("Entity had invalid id");
                continue;
            }

            TransformComponent* transform = entity->GetComponent<TransformComponent>();
            if (transform == nullptr) {
                LOG_ERROR("Entity has invalid transform");
                continue;
            }

            glm::mat4 model = transform->GetTransform();
            application->GetActiveShader()->SetMat4("model", model);

            renderer.Render(*allMeshRenderers[i]->GetMesh());
        }

        application->EndFrame();
    }

    inputManager->Cleanup();
    testModel.DeleteModel();
    entityRegistry->Cleanup();

    application->Shutdown();
    delete application;
    application = nullptr;
    LOG_TRACE("Exiting...");

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