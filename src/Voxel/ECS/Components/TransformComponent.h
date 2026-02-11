#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <Voxel/ECS/Components/HierarchyComponent.h>

struct TransformComponent {
  private:
    bool dirty = true;

  public:
    static constexpr const char* ComponentName = "Transform";
    Entity entity;

    glm::vec3 position{0.0f};
    glm::quat rotation{1, 0, 0, 0};
    glm::vec3 eulerRotation{0, 0, 0};
    glm::vec3 scale{1.0f};

    glm::mat4 localMatrix{1.0f};
    glm::mat4 worldMatrix{1.0f};

    TransformComponent() = default;

    TransformComponent(const glm::vec3& pos, const glm::vec3& eulerDegrees = glm::vec3(0.0f),
                       const glm::vec3& scl = glm::vec3(1.0f))
        : position(pos), scale(scl) {
        SetRotationEulerDegrees(eulerDegrees);
    }

    void SetPosition(const glm::vec3& newPosition) {
        position = newPosition;
        UpdateTransform();
    }

    void AddPosition(const glm::vec3& delta) {
        position += delta;
        UpdateTransform();
    }

    void SetRotation(const glm::quat& newRotation) {
        rotation = newRotation;
        UpdateTransform();
    }

    void AddRotation(const glm::quat& deltaRotation) {
        rotation = deltaRotation * rotation;
        UpdateTransform();
    }

    void SetRotationEulerDegrees(const glm::vec3& eulerDegrees) {
        glm::vec3 radians = glm::radians(eulerDegrees);
        rotation = glm::quat(radians);
        UpdateTransform();
    }

    void AddRotationEulerDegrees(const glm::vec3& deltaDegrees) {
        glm::vec3 radians = glm::radians(deltaDegrees);
        rotation = glm::quat(radians) * rotation;
        UpdateTransform();
    }

    void SetScale(const glm::vec3& newScale) {
        scale = newScale;
        UpdateTransform();
    }

    void UpdateTransform() {
        glm::mat4 t = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 r = glm::toMat4(rotation);
        glm::mat4 s = glm::scale(glm::mat4(1.0f), scale);

        localMatrix = t * r * s;
        MarkDirty();
    }

    void MarkDirty() {
        EntityRegistry* registry = EntityRegistry::GetInstance();
        dirty = true;
        if (!registry->HasComponent<HierarchyComponent>(entity))
            return;
        HierarchyComponent* hierarchy = registry->GetComponent<HierarchyComponent>(entity);

        // When this transform is dirty, we need to update all children recursively
        for (Entity child : hierarchy->children) {
            TransformComponent* childTransform = registry->GetComponent<TransformComponent>(child);
            childTransform->MarkDirty();
        }
    }
    void MarkClean() { dirty = false; }
    bool IsDirty() const { return dirty; }

    glm::vec3 GetRotationEulerDegrees() const { return glm::degrees(glm::eulerAngles(rotation)); }
    glm::quat GetRotationQuat() const { return rotation; }

    void RenderComponentPanel() {
        const float dragSpeedPos = 1.0f;
        const float dragSpeedRot = 1.0f;
        const float dragSpeedScale = 0.01f;

        bool changed = false;

        auto drawAxis = [&](const char* label, ImVec4 color, float& value, float speed) {
            ImGui::BeginGroup();
            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(label);
            ImGui::PopStyleColor();

            ImGui::SameLine();
            ImGui::PushItemWidth(-1);
            bool changed = ImGui::DragFloat(("##" + std::string(label)).c_str(), &value, speed,
                                            0.0f, 0.0f, "%.1f", ImGuiSliderFlags_NoRoundToFormat);

            ImGui::PopItemWidth();
            ImGui::EndGroup();
            return changed;
        };

        ImGui::SeparatorText("Position");
        ImGui::Spacing();

        ImGui::PushID("Position");

        changed |= drawAxis("X", ImVec4(0.90f, 0.25f, 0.25f, 1.0f), position.x, dragSpeedPos);
        changed |= drawAxis("Y", ImVec4(0.25f, 0.90f, 0.25f, 1.0f), position.y, dragSpeedPos);
        changed |= drawAxis("Z", ImVec4(0.25f, 0.45f, 0.90f, 1.0f), position.z, dragSpeedPos);

        ImGui::PopID();

        ImGui::SeparatorText("Rotation");
        ImGui::Spacing();
        ImGui::PushID("Rotation");

        changed |= drawAxis("X", ImVec4(0.90f, 0.25f, 0.25f, 1.0f), eulerRotation.x, dragSpeedRot);
        changed |= drawAxis("Y", ImVec4(0.25f, 0.90f, 0.25f, 1.0f), eulerRotation.y, dragSpeedRot);
        changed |= drawAxis("Z", ImVec4(0.25f, 0.45f, 0.90f, 1.0f), eulerRotation.z, dragSpeedRot);

        ImGui::PopID();

        ImGui::SeparatorText("Scale");
        ImGui::Spacing();
        ImGui::PushID("Scale");

        changed |= drawAxis("X", ImVec4(0.90f, 0.25f, 0.25f, 1.0f), scale.x, dragSpeedScale);
        changed |= drawAxis("Y", ImVec4(0.25f, 0.90f, 0.25f, 1.0f), scale.y, dragSpeedScale);
        changed |= drawAxis("Z", ImVec4(0.25f, 0.45f, 0.90f, 1.0f), scale.z, dragSpeedScale);

        ImGui::PopID();

        if (changed)
            SetRotationEulerDegrees(eulerRotation);
    }
};
