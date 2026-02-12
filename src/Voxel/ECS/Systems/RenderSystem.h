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
                                                0.1f, 10000.0f);

        application->GetActiveShader()->SetMat4("view", view);
        application->GetActiveShader()->SetMat4("projection", projection);

        std::unordered_map<RawModel*, std::vector<glm::mat4>> batches;
        for (auto [e, transform, mesh, meta] :
             entityRegistry
                 ->MakeView<const TransformComponent, const MeshComponent, const MetaComponent>()) {
            if (!meta.effectiveVisibility)
                continue;

            if (!mesh.model) {
                LOG_ERROR("Mesh not found");
                continue;
            }

            batches[mesh.model].push_back(transform.worldMatrix);
        }
        for (auto& [model, transforms] : batches) {
            renderer.Bind(*model);
            for (const glm::mat4& matrix : transforms) {

                application->GetActiveShader()->SetMat4("model", matrix);
                renderer.Render(*model);
            }
            renderer.Unbind();
        }
    }

  private:
    static inline Camera* camera = nullptr;
    static inline Application* application = nullptr;
    static inline EntityRegistry* entityRegistry = nullptr;
    static inline EntityRenderer renderer = EntityRenderer();
};