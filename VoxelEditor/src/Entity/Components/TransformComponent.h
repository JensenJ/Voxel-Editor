#pragma once
#include "Component.h"
#include <glm/glm.hpp>

class TransformComponent : public Component
{
public:

	TransformComponent(glm::vec3 position);
	TransformComponent(glm::vec3 position, glm::vec3 rotation);
	TransformComponent(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);

	glm::vec3 GetPosition();
	glm::vec3 GetRotation();
	glm::vec3 GetScale();
	
	glm::mat4 GetTransform();
	
	void SetPosition(glm::vec3 newPos);
	void SetRotation(glm::vec3 newRot);
	void SetScale(glm::vec3 newScale);

	void AddPosition(glm::vec3 addedPosition);
	void AddRotation(glm::vec3 addedRotation);

private:
	glm::mat4 transform = glm::mat4(1.0f);
	glm::vec3 rotationEuler = glm::vec3(0.0f);	//Easier to keep copy than to extract from transform matrix, must update whenever rotation is applied
};

