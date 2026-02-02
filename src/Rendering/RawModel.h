#pragma once

#include <vector>
#include <glm/glm.hpp>

struct Vertex {
	Vertex(glm::vec3 position, glm::vec3 colour)
	{
		this->position = position;
		this->colour = colour;
	}

	glm::vec3 position;
	glm::vec3 colour;
};

class RawModel
{
public:
	RawModel(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

	unsigned int GetVAO();
	int GetVertexCount();
	void DeleteModel();
private:
	void CreateModel();

	unsigned int VAO, VBO, EBO;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
};

