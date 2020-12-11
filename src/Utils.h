#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <sstream>
#include "Model.h"
#include "Plane.h"

using namespace std;

string to_string(glm::vec3 v)
{
    std::ostringstream ss;
    ss << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return ss.str();
}

string to_string(glm::vec4 v)
{
    std::ostringstream ss;
    ss << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
    return ss.str();
}

Model* makeStaticCube(float length, glm::vec3 pos, glm::mat4x4 rotation, Texture* texture)
{
    float cubeVertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    float half_length = length / 2;
    constexpr int count = sizeof(cubeVertices) / sizeof(float) / 3;
    for (int i = 0; i < count * 3; i++)
    {
        cubeVertices[i] *= half_length;
    }

    Vertex* vertices = new Vertex[count];
    for (int i = 0; i < count; i++)
    {
        int k = 3 * i;
        vertices[i] = { { cubeVertices[k], cubeVertices[k + 1], cubeVertices[k + 2] }, { cubeVertices[k] / length + 0.5f, cubeVertices[k + 1] / length + 0.5f } };
        vertices[i].position = glm::vec3(rotation * glm::translate(pos) * glm::vec4(vertices[i].position, 1));
    }

	return new Model(count, vertices, texture);
}