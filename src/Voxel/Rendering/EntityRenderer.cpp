#include "EntityRenderer.h"
#include <Voxel/pch.h>
#include <Voxel/Core.h>

void EntityRenderer::Render(RawModel& modelToRender) {
    glBindVertexArray(modelToRender.GetVAO());
    glDrawElements(GL_TRIANGLES, modelToRender.GetVertexCount(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
