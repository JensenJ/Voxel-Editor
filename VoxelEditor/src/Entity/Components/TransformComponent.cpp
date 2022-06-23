#include "TransformComponent.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <iostream>

TransformComponent::TransformComponent(glm::vec3 position)
{
	SetPosition(position);
}

TransformComponent::TransformComponent(glm::vec3 position, glm::vec3 rotation)
{
	SetPosition(position);
	SetRotation(rotation);
}

TransformComponent::TransformComponent(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
	SetPosition(position);
	SetRotation(rotation);
	SetScale(scale);
}

glm::vec3 TransformComponent::GetPosition()
{
	return transform[3];
}

void TransformComponent::SetPosition(glm::vec3 newPosition)
{
	//Go back to 0, 0, 0, then translate to new position
	transform = glm::translate(transform, -GetPosition() + newPosition);
}

void TransformComponent::AddPosition(glm::vec3 addedPosition)
{
	//Translate by added position
	transform = glm::translate(transform, addedPosition);
}

glm::vec3 TransformComponent::GetRotation()
{
	return rotationEuler;
}

void TransformComponent::SetRotation(glm::vec3 newRotation)
{
	//std::cout << "rotation before reset:  " << GetRotation().x << ", " << GetRotation().y << ", " << GetRotation().z << std::endl;

	//Remove old rotation in opposite order to being set to return to 0, 0, 0
	transform = glm::rotate(transform, glm::radians(-GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
	transform = glm::rotate(transform, glm::radians(-GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(-GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));

	//std::cout << "reset rotation to:  " << GetRotation().x << ", " << GetRotation().y << ", " << GetRotation().z << std::endl;

	//apply new rotation
	transform = glm::rotate(transform, glm::radians(newRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(newRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	transform = glm::rotate(transform, glm::radians(newRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

	rotationEuler = newRotation;
}

void TransformComponent::AddRotation(glm::vec3 addedRotation)
{
	SetRotation(GetRotation() + addedRotation);
}

//Get the scale of this object
glm::vec3 TransformComponent::GetScale()
{
	glm::vec3 scale = glm::vec3();
	for (int i = 0; i < 3; i++)
	{
		scale[i] = glm::length(transform[i]);
	}
	return scale;
}

void TransformComponent::SetScale(glm::vec3 newScale)
{
	//Create scale which will revert back to scale size 1
	glm::vec3 resetScaleSize = glm::vec3(1 / GetScale().x, 1 / GetScale().y, 1 / GetScale().z);
	transform = glm::scale(transform, resetScaleSize);

	//Apply new scale from scale 1
	transform = glm::scale(transform, newScale);
}

glm::mat4 TransformComponent::GetTransform()
{
	return transform;
}