#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#pragma pack(0)
struct Vertex
{
    glm::vec3 position;
    glm::vec2 texcoords;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;

    Vertex(glm::vec3 position = glm::vec3(), glm::vec2 texcoords = glm::vec2(),
        glm::vec3 normal = glm::vec3(), glm::vec3 tangent = glm::vec3(), glm::vec3 bitangent = glm::vec3())
            : position(position), texcoords(texcoords), normal(normal), tangent(tangent), bitangent(bitangent) {}
};
#pragma pack()