#pragma once
#include <Voxel/pch.h>

struct Vertex {
    Vertex(glm::vec3 position, glm::vec3 colour) {
        this->position = position;
        this->colour = colour;
    }

    glm::vec3 position;
    glm::vec3 colour;
};

class RawModel {
  public:
    RawModel(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

    unsigned int GetVAO() const;
    unsigned int GetIndexCount() const;
    void DeleteModel();

    void UpdateInstanceBuffer(const std::vector<glm::mat4>& matrices);

  private:
    void CreateModel();

    unsigned int VAO, VBO, EBO;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    GLuint instanceVBO = 0;
    size_t instanceCapacity = 0;
};
