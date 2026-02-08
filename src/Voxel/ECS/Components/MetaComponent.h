#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>

struct MetaComponent {
    Entity entity;
    std::string name{"Entity"};
    bool visible{true};

    MetaComponent() = default;
    MetaComponent(const std::string& n, bool vis = true) : name(n), visible(vis) {}
};
