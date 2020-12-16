#pragma once

#include <GL/glew.h>
#include <iostream>
#include "Model.h"
#include "Texture.h"

class TexturedModel
{
public:
    Model* model;
    Texture* texture;

    TexturedModel(Model* model, Texture* texture)
    {
        this->model = model;
        this->texture = texture;
    }

    static Vertex* createVertices(int size, glm::vec3* modelPoses, glm::vec2* texCoords)
    {
        Vertex* vertices = new Vertex[size];
        for (int i = 0; i < size; i += 3)
        {
            glm::vec2 dUV1 = texCoords[i + 1] - texCoords[i];
            glm::vec2 dUV2 = texCoords[i + 2] - texCoords[i];
            float dU1 = dUV1.x, dV1 = dUV1.y;
            float dU2 = dUV2.x, dV2 = dUV2.y;
            float D_1 = 1 / (dU1 * dV2 - dU2 * dV1);
            glm::mat2x2 coefs = { dV2, -dU2, -dV1, dU1 };
            glm::vec3 dP1 = modelPoses[i + 1] - modelPoses[i];
            glm::vec3 dP2 = modelPoses[i + 2] - modelPoses[i];
            glm::mat3x2 models = { dP1.x, dP2.x, dP1.y, dP2.y, dP1.z, dP2.z };
            glm::mat3x2 TB = D_1 * coefs * models;
            glm::vec3 tangent = { TB[0][0], TB[1][0], TB[2][0] };
            glm::vec3 bitangent = { TB[0][1], TB[1][1], TB[2][1] };
            tangent = glm::normalize(tangent);
            bitangent = glm::normalize(bitangent);
            glm::vec3 normal = glm::cross(tangent, bitangent);
            vertices[i + 0] = { modelPoses[i + 0], texCoords[i + 0], normal, tangent, bitangent };
            vertices[i + 1] = { modelPoses[i + 1], texCoords[i + 1], normal, tangent, bitangent };
            vertices[i + 2] = { modelPoses[i + 2], texCoords[i + 2], normal, tangent, bitangent };
        }
        return vertices;
    }

    void draw()
    {
        texture->use();
        model->draw();
    }
};