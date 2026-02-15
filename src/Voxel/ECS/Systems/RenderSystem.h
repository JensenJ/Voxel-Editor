#pragma once

#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <typeindex>
#include <Voxel/Camera.h>
#include <Voxel/ECS/Components/MeshComponent.h>
#include <Voxel/ECS/Components/TransformComponent.h>
#include <Voxel/ECS/Systems/TransformSystem.h>
#include <Voxel/ECS/Systems/VisibilitySystem.h>
#include <Voxel/Rendering/RawModelRenderer.h>

struct ModelBatch {
    std::vector<glm::mat4> transforms;
    std::vector<Entity> entities;
    std::unordered_map<Entity, size_t> slots;
    bool dirty = true;
};

class RenderSystem {
  public:
    static void Init(Application* app, Camera* cam, EntityRegistry* registry) {
        application = app;
        camera = cam;
        entityRegistry = registry;

        VisibilitySystem::onEntityChangedEffectiveVisibility.AddObserver(
            [](const EntityVisibilityChangedEvent& event) {
                if (event.visibility)
                    AddEntityToBatch(event.entity);
                else
                    RemoveEntityFromBatch(event.entity);
            });

        TransformSystem::onEntityChangedWorldTransform.AddObserver(
            [](const EntityChangedTransformEvent& event) {
                MeshComponent* mesh = entityRegistry->GetComponent<MeshComponent>(event.entity);
                TransformComponent* transform =
                    entityRegistry->GetComponent<TransformComponent>(event.entity);

                if (!mesh || !transform)
                    return;

                auto& batch = batches[mesh->model];

                auto it = batch.slots.find(event.entity);
                if (it == batch.slots.end())
                    return;

                batch.transforms[it->second] = event.newTransform;
                batch.dirty = true;
            });

        EntityRegistry::onAddComponent.AddObserver([](const EntityAddComponentEvent& event) {
            if (event.componentType == std::type_index(typeid(MeshComponent)))
                AddEntityToBatch(event.entity);
        });

        EntityRegistry::onRemoveComponent.AddObserver([](const EntityRemoveComponentEvent& event) {
            if (event.componentType == std::type_index(typeid(MeshComponent)))
                RemoveEntityFromBatch(event.entity);
        });

        EntityRegistry::onClearEntities.AddObserver(
            [](const EntityClearEvent& event) { batches.clear(); });

        LOG_INFO("Initialised RenderSystem");
    }

    static void Run();

    static void AddEntityToBatch(Entity e);
    static void RemoveEntityFromBatch(Entity e);

  private:
    static inline std::unordered_map<RawModel*, ModelBatch> batches =
        std::unordered_map<RawModel*, ModelBatch>();

    static inline Camera* camera = nullptr;
    static inline Application* application = nullptr;
    static inline EntityRegistry* entityRegistry = nullptr;
    static inline RawModelRenderer rawModelRenderer = RawModelRenderer();
};