#include "RenderSystem.h"
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/ECS/Components/MeshComponent.h>
#include <Voxel/ECS/Components/MetaComponent.h>
#include <Voxel/ECS/Components/TransformComponent.h>
#include <Voxel/Rendering/ShaderLoader.h>

void RenderSystem::Run() {
    ScopedTimer timer(Profiler::system_render);
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera->GetZoom()),
                                            (float)application->GetSceneViewportWidth() /
                                                (float)application->GetSceneViewportHeight(),
                                            0.1f, 10000.0f);

    application->GetActiveShader()->SetMat4("view", view);
    application->GetActiveShader()->SetMat4("projection", projection);

    std::unordered_map<RawModel*, std::vector<glm::mat4>> batches;
    {
        ScopedTimer timer(Profiler::system_render_batching);
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
    }
    {
        ScopedTimer timer(Profiler::system_render_draw);
        for (auto& [model, transforms] : batches) {
            model->UpdateInstanceBuffer(transforms);
            rawModelRenderer.Bind(*model);
            rawModelRenderer.Render(*model, transforms.size());
            rawModelRenderer.Unbind();
        }
    }
}