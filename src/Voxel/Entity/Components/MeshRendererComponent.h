#pragma once
#include <Voxel/pch.h>
#include <Voxel/Rendering/RawModel.h>
#include "Component.h"

class MeshRendererComponent : public Component {
  public:
    void SetMesh(RawModel* mesh);

    MeshRendererComponent(RawModel* mesh);

    RawModel* GetMesh();

  private:
    RawModel* meshToRender = nullptr;
};
