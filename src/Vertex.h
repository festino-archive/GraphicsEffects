#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#pragma pack(0)
struct Vertex
{
    glm::vec3 position;
    glm::vec2 texcoords;
};
#pragma pack()