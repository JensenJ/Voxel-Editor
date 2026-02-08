#pragma once
#include <Voxel/pch.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <Voxel/ECS/Entity.h>

struct TransformComponent {
    Entity entity;

    glm::vec3 position{0.0f};
    glm::quat rotation{1, 0, 0, 0}; // Identity quaternion
    glm::vec3 scale{1.0f};

    glm::mat4 transform{1.0f};

    // Default constructor
    TransformComponent() = default;

    // Constructor using Euler angles in DEGREES
    TransformComponent(const glm::vec3& pos, const glm::vec3& eulerDegrees = glm::vec3(0.0f),
                       const glm::vec3& scl = glm::vec3(1.0f))
        : position(pos), scale(scl) {
        SetRotationEulerDegrees(eulerDegrees);
    }

    // Position
    void SetPosition(const glm::vec3& newPosition) {
        position = newPosition;
        UpdateTransform();
    }

    void AddPosition(const glm::vec3& delta) {
        position += delta;
        UpdateTransform();
    }

    // Rotation using quaternions
    void SetRotation(const glm::quat& newRotation) {
        rotation = newRotation;
        UpdateTransform();
    }

    void AddRotation(const glm::quat& deltaRotation) {
        rotation = deltaRotation * rotation; // quaternion multiplication
        UpdateTransform();
    }

    // Rotation using Euler angles in DEGREES
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

    // Scale
    void SetScale(const glm::vec3& newScale) {
        scale = newScale;
        UpdateTransform();
    }

    // Update the final transform matrix
    void UpdateTransform() {
        glm::mat4 t = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 r = glm::toMat4(rotation);
        glm::mat4 s = glm::scale(glm::mat4(1.0f), scale);

        transform = t * r * s;
    }

    // Convenience getters
    glm::vec3 GetRotationEulerDegrees() const { return glm::degrees(glm::eulerAngles(rotation)); }
    glm::quat GetRotationQuat() const { return rotation; }

    /*void TransformComponent::RenderPropertiesPanel() {
        ImGui::SeparatorText("Position");
        ImGui::Spacing();

        ImGui::PushID("Position");

        const float labelWidth = 16.0f;
        const float dragSpeed = 1.0f;

        auto drawAxis = [&](const char* label, ImVec4 color, float& value) {
            ImGui::BeginGroup();
            ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(label);
            ImGui::PopStyleColor();

            ImGui::SameLine();
            ImGui::PushItemWidth(-1);
            ImGui::DragFloat(("##" + std::string(label)).c_str(), &value, dragSpeed, 0.0f, 0.0f,
                             "%.0f");
            ImGui::PopItemWidth();
            ImGui::EndGroup();
        };

        drawAxis("X", ImVec4(0.90f, 0.25f, 0.25f, 1.0f), transform[3].x);
        drawAxis("Y", ImVec4(0.25f, 0.90f, 0.25f, 1.0f), transform[3].y);
        drawAxis("Z", ImVec4(0.25f, 0.45f, 0.90f, 1.0f), transform[3].z);

        ImGui::PopID();
    }*/
};
