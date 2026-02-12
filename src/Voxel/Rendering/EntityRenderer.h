#pragma once

#include "RawModel.h"

class EntityRenderer {
  public:
    void Render(RawModel& modelToRender);
    void Bind(RawModel& modelToBind);
    void Unbind();
};
