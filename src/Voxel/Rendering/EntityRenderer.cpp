#include "EntityRenderer.h"
#include <Voxel/pch.h>
#include <Voxel/Core.h>

void EntityRenderer::Bind(RawModel& modelToBind) { glBindVertexArray(modelToBind.GetVAO()); }

void EntityRenderer::Render(RawModel& modelToRender) {
    glDrawElements(GL_TRIANGLES, modelToRender.GetVertexCount(), GL_UNSIGNED_INT, 0);
}

void EntityRenderer::Unbind() { glBindVertexArray(0); }
