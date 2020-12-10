#pragma once

#include <glm/vec3.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/mat4x4.hpp>

class Plane
{
    glm::vec3 a;
    glm::vec3 b;
    glm::vec3 normal;
    glm::vec3 height;
    float d;

public:
    Plane(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
    {
        a = { p2 - p1 };
        b = { p3 - p1 };
        normal = glm::cross(a, b);
        float dp = a.x * b.y * normal.z + a.y * b.z * normal.x + a.z * b.x * normal.y;
        float dn = a.z * b.y * normal.x + a.x * b.z * normal.y + a.y * b.x * normal.z;
        d = dp - dn;
        float dzp = a.x * b.y * p1.z + a.y * b.z * p1.x + a.z * b.x * p1.y;
        float dzn = a.z * b.y * p1.x + a.x * b.z * p1.y + a.y * b.x * p1.z;
        float dz = dzp - dzn;
        height = (dz / d) * normal;
        if (glm::dot(p1, height) < 0)
            height = -height;
    }

    glm::vec3 project(glm::vec3 point)
    {
        point -= height;
        float dxp = point.x * b.y * normal.z + point.y * b.z * normal.x + point.z * b.x * normal.y;
        float dxn = point.z * b.y * normal.x + point.x * b.z * normal.y + point.y * b.x * normal.z;
        float dx = dxp - dxn;
        float dyp = a.x * point.y * normal.z + a.y * point.z * normal.x + a.z * point.x * normal.y;
        float dyn = a.z * point.y * normal.x + a.x * point.z * normal.y + a.y * point.x * normal.z;
        float dy = dyp - dyn;
        /*float dzp = a.x * b.y * point.z + a.y * b.z * point.x + a.z * b.x * point.y;
        float dzn = a.z * b.y * point.x + a.x * b.z * point.y + a.y * b.x * point.z;
        float dz = dzp - dzn;*/
        float k1 = dx / d;
        float k2 = dy / d;
        return height + k1 * a + k2 * b;
    }

    glm::vec3 flip(glm::vec3 point)
    {
        return 2.0f * project(point) - point;
    }

    glm::mat4x4 flipRotation(glm::mat4x4 rot)
    {
        rot = glm::transpose(rot);
        glm::vec3 center = glm::vec3(0, 0, 0);
        glm::vec3 right = glm::vec3(rot * glm::vec4(1, 0, 0, 1));
        glm::vec3 up = glm::vec3(rot * glm::vec4(0, 1, 0, 1));
        glm::vec3 backward = glm::vec3(rot * glm::vec4(0, 0, 1, 1));
        center = flip(center);
        right = flip(right) - center;
        up = flip(up) - center;
        backward = flip(backward) - center;
        glm::mat4x4 res = {
            glm::vec4(right, 0),
            glm::vec4(up, 0),
            glm::vec4(backward, 0),
            glm::vec4(0, 0, 0, 1)
        };
        return glm::transpose(res);
    }
};