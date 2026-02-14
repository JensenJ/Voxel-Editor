#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>
#include <Voxel/Rendering/RawModel.h>

struct MeshComponent {
    static constexpr const char* ComponentName = "Mesh";
    Entity entity;
    RawModel* model = nullptr;

    MeshComponent() = default;
    explicit MeshComponent(Entity entity, RawModel* m) : entity(entity), model(m) {}

    void RenderComponentPanel() { ImGui::Text("Vertices: %d", model->GetIndexCount()); }
};
