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

    for (auto& [model, batch] : batches) {
        if (batch.transforms.empty())
            continue;

        if (batch.dirty) {
            model->UpdateInstanceBuffer(batch.transforms);
            batch.dirty = false;
        }

        rawModelRenderer.Bind(*model);
        rawModelRenderer.Render(*model, batch.transforms.size());
        rawModelRenderer.Unbind();
    }
}

void RenderSystem::AddEntityToBatch(Entity e) {
    MeshComponent* mesh = entityRegistry->GetComponent<MeshComponent>(e);
    TransformComponent* transform = entityRegistry->GetComponent<TransformComponent>(e);

    if (!mesh || !transform)
        return;

    auto& batch = batches[mesh->model];

    size_t slot = batch.transforms.size();

    batch.transforms.push_back(transform->worldMatrix);
    batch.entities.push_back(e);
    batch.slots[e] = slot;
    batch.dirty = true;
}

void RenderSystem::RemoveEntityFromBatch(Entity e) {
    MeshComponent* mesh = entityRegistry->GetComponent<MeshComponent>(e);
    if (!mesh)
        return;

    auto itBatch = batches.find(mesh->model);
    if (itBatch == batches.end())
        return;

    auto& batch = itBatch->second;

    auto itSlot = batch.slots.find(e);
    if (itSlot == batch.slots.end())
        return;

    size_t slot = itSlot->second;
    size_t lastIndex = batch.transforms.size() - 1;

    if (slot != lastIndex) {
        batch.transforms[slot] = batch.transforms[lastIndex];
        batch.entities[slot] = batch.entities[lastIndex];
        batch.slots[batch.entities[slot]] = slot;
    }

    batch.transforms.pop_back();
    batch.entities.pop_back();
    batch.slots.erase(itSlot);
    batch.dirty = true;
}