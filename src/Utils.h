#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <iostream>
#include <sstream>
#include "Model.h"

using namespace std;

string to_string(glm::vec3 v)
{
    std::ostringstream ss;
    ss << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return ss.str();
}

glm::vec3 project(glm::vec3 point, glm::vec3 plane1, glm::vec3 plane2)
{
    glm::vec3 normal = glm::cross(plane1, plane2);
    float k1, k2, h;
    float dp = plane1.x * plane2.y * normal.z + plane1.y * plane2.z * normal.x + plane1.z * plane2.x * normal.y;
    float dn = plane1.z * plane2.y * normal.x + plane1.x * plane2.z * normal.y + plane1.y * plane2.x * normal.z;
    float d = dp - dn;
    float dxp = point.x * plane2.y * normal.z + point.y * plane2.z * normal.x + point.z * plane2.x * normal.y;
    float dxn = point.z * plane2.y * normal.x + point.x * plane2.z * normal.y + point.y * plane2.x * normal.z;
    float dx = dxp - dxn;
    float dyp = plane1.x * point.y * normal.z + plane1.y * point.z * normal.x + plane1.z * point.x * normal.y;
    float dyn = plane1.z * point.y * normal.x + plane1.x * point.z * normal.y + plane1.y * point.x * normal.z;
    float dy = dyp - dyn;
    float dzp = plane1.x * plane2.y * point.z + plane1.y * plane2.z * point.x + plane1.z * plane2.x * point.y;
    float dzn = plane1.z * plane2.y * point.x + plane1.x * plane2.z * point.y + plane1.y * plane2.x * point.z;
    float dz = dzp - dzn;
    k1 = dx / d;
    k2 = dy / d;
    h = dz / d;
    return k1 * plane1 + k2 * plane2;
}

glm::vec3 flip(glm::vec3 point, glm::vec3 plane1, glm::vec3 plane2)
{
    return 2.0f * project(point, plane1, plane2) - point;
}

glm::mat4x4 flipRotation(glm::mat4x4 rot, glm::vec3 plane1, glm::vec3 plane2)
{
    rot = glm::transpose(rot);
    glm::vec3 center = glm::vec3(0, 0, 0);
    glm::vec3 right = glm::vec3(rot * glm::vec4(1, 0, 0, 1));
    glm::vec3 up = glm::vec3(rot * glm::vec4(0, 1, 0, 1));
    glm::vec3 backward = glm::vec3(rot * glm::vec4(0, 0, 1, 1));
    center = flip(center, plane1, plane2);
    right = flip(right, plane1, plane2) - center;
    up = flip(up, plane1, plane2) - center;
    backward = flip(backward, plane1, plane2) - center;
    /*glm::mat4x4 res = {
        right.x, up.x, backward.x, 0,
        right.y, up.y, backward.y, 0,
        right.z, up.z, backward.z, 0,
        0, 0, 0, 1
    };*/
    glm::mat4x4 res = {
        glm::vec4(right, 0),
        glm::vec4(up, 0),
        glm::vec4(backward, 0),
        glm::vec4(0, 0, 0, 1)
    };
    //cout << to_string(right) << " == " << to_string(res * glm::vec4(1, 0, 0, 1)) << endl;
    //cout << to_string(up) << " == " << to_string(res * glm::vec4(0, 1, 0, 1)) << endl;
    //cout << to_string(backward) << " == " << to_string(res * glm::vec4(0, 0, 1, 1)) << endl;
    return glm::transpose(res);
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