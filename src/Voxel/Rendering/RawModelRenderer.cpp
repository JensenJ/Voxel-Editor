#include "RawModelRenderer.h"
#include <Voxel/pch.h>
#include <Voxel/Core.h>

void RawModelRenderer::Bind(RawModel& modelToBind) { glBindVertexArray(modelToBind.GetVAO()); }

void RawModelRenderer::Render(RawModel& modelToRender, size_t instanceCount) {

    glDrawElementsInstanced(GL_TRIANGLES, modelToRender.GetIndexCount(), GL_UNSIGNED_INT, 0,
                            static_cast<GLsizei>(instanceCount));
}

void RawModelRenderer::Unbind() { glBindVertexArray(0); }
