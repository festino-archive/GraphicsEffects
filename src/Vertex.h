#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#pragma pack(0)
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    //glm::vec2 texcoords;
    //glm::vec3 tangent;
    //glm::vec3 binormal;
};
#pragma pack()