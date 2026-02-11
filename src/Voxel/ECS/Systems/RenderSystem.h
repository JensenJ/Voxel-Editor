#pragma once

#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/Camera.h>
#include <Voxel/Rendering/EntityRenderer.h>
#include <Voxel/Rendering/ShaderLoader.h>

class RenderSystem {
  public:
    static void Init(Application* app, Camera* cam, EntityRegistry* registry) {
        application = app;
        camera = cam;
        entityRegistry = registry;
        LOG_INFO("Initialised RenderSystem");
    }

    static void Run() {
        glm::mat4 view = camera->GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera->GetZoom()),
                                                (float)application->GetSceneViewportWidth() /
                                                    (float)application->GetSceneViewportHeight(),
                                                0.1f, 100.0f);

        application->GetActiveShader()->SetMat4("view", view);
        application->GetActiveShader()->SetMat4("projection", projection);

        for (auto [e, transform, mesh, meta] :
             entityRegistry
                 ->MakeView<const TransformComponent, const MeshComponent, const MetaComponent>()) {

            if (!meta.visible)
                continue;

            application->GetActiveShader()->SetMat4("model", transform.worldMatrix);
            if (mesh.model)
                renderer.Render(*mesh.model);
            else
                LOG_ERROR("Mesh not found");
        }
    }

  private:
    static inline Camera* camera = nullptr;
    static inline Application* application = nullptr;
    static inline EntityRegistry* entityRegistry = nullptr;
    static inline EntityRenderer renderer = EntityRenderer();
};