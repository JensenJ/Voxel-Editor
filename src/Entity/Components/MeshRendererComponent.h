#pragma once
#include <glm/glm.hpp>
#include "../../Rendering/RawModel.h"
#include "Component.h"

class MeshRendererComponent : public Component {
  public:
    void SetMesh(RawModel* mesh);

    MeshRendererComponent(RawModel* mesh);

    RawModel* GetMesh();

  private:
    RawModel* meshToRender = nullptr;
};
