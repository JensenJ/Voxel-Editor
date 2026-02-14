#include "RawModel.h"
#include <Voxel/pch.h>
#include <Voxel/Core.h>

RawModel::RawModel(std::vector<Vertex> vertices, std::vector<unsigned int> indices)
    : vertices(std::move(vertices)), indices(std::move(indices)) {
    CreateModel();
}

unsigned int RawModel::GetVAO() const { return VAO; }

unsigned int RawModel::GetIndexCount() const { return (unsigned int)indices.size(); }

void RawModel::DeleteModel() {
    glDeleteBuffers(1, &instanceVBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void RawModel::CreateModel() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(),
                 GL_STATIC_DRAW);

    // Store indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(),
                 GL_STATIC_DRAW);

    // Store positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Store colours
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, colour));

    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

    instanceCapacity = 1;
    glBufferData(GL_ARRAY_BUFFER, instanceCapacity * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);

    // A mat4 takes 4 attribute locations
    for (int i = 0; i < 4; ++i) {
        glEnableVertexAttribArray(2 + i);
        glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                              (void*)(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(2 + i, 1);
    }

    glBindVertexArray(0);
}

void RawModel::UpdateInstanceBuffer(const std::vector<glm::mat4>& matrices) {
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

    // Resize if necessary
    if (matrices.size() > instanceCapacity) {
        instanceCapacity = matrices.size();

        glBufferData(GL_ARRAY_BUFFER, instanceCapacity * sizeof(glm::mat4), nullptr,
                     GL_DYNAMIC_DRAW);
    }

    glBufferSubData(GL_ARRAY_BUFFER, 0, matrices.size() * sizeof(glm::mat4), matrices.data());
}
