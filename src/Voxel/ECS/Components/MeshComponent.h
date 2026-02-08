#pragma once
#include <Voxel/pch.h>
#include <Voxel/ECS/Entity.h>
#include <Voxel/Rendering/RawModel.h>

struct MeshComponent {
    Entity entity;
    RawModel* model = nullptr;

    MeshComponent() = default;
    explicit MeshComponent(RawModel* m) : model(m) {}
};
