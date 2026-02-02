#pragma once
#include "Component.h"
#include "../../Rendering/RawModel.h"
#include <glm/glm.hpp>

class MeshRendererComponent : public Component
{
public:
	void SetMesh(RawModel* mesh);

	MeshRendererComponent(RawModel* mesh);

	RawModel* GetMesh();
private:
	RawModel* meshToRender = nullptr; 
};

