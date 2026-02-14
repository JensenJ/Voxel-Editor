#pragma once

#include "RawModel.h"

class RawModelRenderer {
  public:
    void Render(RawModel& modelToRender, size_t instanceCount);
    void Bind(RawModel& modelToBind);
    void Unbind();
};
