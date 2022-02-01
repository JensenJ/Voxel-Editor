#pragma once

#include <vector>
#include <glm/glm.hpp>

struct Vertex {
	Vertex(glm::vec<3, double> position, glm::vec<3, float> colour)
	{
		this->position = position;
		this->colour = colour;
	}

	glm::vec<3, double> position;
	glm::vec<3, float> colour;
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

