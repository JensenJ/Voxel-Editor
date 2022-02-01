#include "EntityRenderer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void EntityRenderer::Render(RawModel& modelToRender)
{
	glBindVertexArray(modelToRender.GetVAO());
	glDrawElements(GL_TRIANGLES, modelToRender.GetVertexCount(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
