#pragma once

#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <glm/gtx/matrix_decompose.hpp>

struct EntityChangedParentEvent {
    Entity entity;
    Entity newParent;
    Entity oldParent;
};

struct EntityChangedTransformEvent {
    Entity entity;
    glm::mat4 oldTransform;
    glm::mat4 newTransform;
};

class TransformSystem {
  public:
    static void Init(EntityRegistry* registry) {
        entityRegistry = registry;

        onEntityChangedParent.AddObserver(
            [](const EntityChangedParentEvent& event) { dirtyEntities.push_back(event.entity); });

        onEntityChangedLocalTransform.AddObserver([](const EntityChangedTransformEvent& event) {
            dirtyEntities.push_back(event.entity);
        });

        LOG_INFO("Initialised TransformSystem");
    }

    static inline Subject<EntityChangedParentEvent> onEntityChangedParent;
    static inline Subject<EntityChangedTransformEvent> onEntityChangedLocalTransform;
    static inline Subject<EntityChangedTransformEvent> onEntityChangedWorldTransform;

    static void Run();
    static void Reparent(Entity child, Entity newParent);
    static bool IsDescendant(Entity possibleParent, Entity entity);

  private:
    static inline EntityRegistry* entityRegistry = nullptr;
    static inline std::vector<Entity> dirtyEntities = std::vector<Entity>();

    static void UpdateRecursive(Entity entity, const glm::mat4& parentWorld);

    static void DecomposeTransform(const glm::mat4& mat, glm::vec3& position, glm::quat& rotation,
                                   glm::vec3& scale) {
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(mat, scale, rotation, position, skew, perspective);
        rotation = glm::normalize(rotation);
    }
};
