#pragma once

#include <glm/vec3.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/mat4x4.hpp>
#include <array>

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
        normal = glm::normalize(normal);
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

    static inline float sgn(float a)
    {
        if (a > 0.0F) return (1.0F);
        if (a < 0.0F) return (-1.0F);
        return (0.0F);
    }

    glm::mat4x4 clipNearPlane(glm::mat4x4 proj)
    {
        glm::vec4 clipPlane = getFullNormal();
        if (clipPlane.w > 0)
            clipPlane = -clipPlane;
        glm::vec4 q = glm::inverse(proj) * glm::vec4(
            sgn(clipPlane.x),
            sgn(clipPlane.y),
            1.0f,
            1.0f
        );
        glm::vec4 c = clipPlane * (2.0F / (glm::dot(clipPlane, q)));
        // third row = clip plane - fourth row
        proj[0][2] = c.x - proj[0][3];
        proj[1][2] = c.y - proj[1][3];
        proj[2][2] = c.z - proj[2][3];
        proj[3][2] = c.w - proj[3][3];
        return proj;
    }

    std::array<glm::vec3, 3> getPosAndBasis()
    {
        return { height, a, b };
    }

    glm::vec4 getFullNormal()
    {
        return glm::vec4(normal, -glm::length(height));
    }

    glm::mat4x4 getReflectionMatrix()
    {
        return glm::identity<glm::mat4x4>() - 2.0f * glm::outerProduct(glm::vec4(normal, 0.0f), glm::vec4(normal, 0.0f));
    }

    glm::vec3 intersection(glm::vec3 point, glm::vec3 base)
    {
        float dp = a.x * b.y * base.z + a.y * b.z * base.x + a.z * b.x * base.y;
        float dn = a.z * b.y * base.x + a.x * b.z * base.y + a.y * b.x * base.z;
        float d = dp - dn;
        glm::vec3 right = point - height;
        float dbazep = a.x * b.y * right.z + a.y * b.z * right.x + a.z * b.x * right.y;
        float dbazen = a.z * b.y * right.x + a.x * b.z * right.y + a.y * b.x * right.z;
        float dbaze = dbazep - dbazen;
        return point + base * (dbaze / d);
    }

    glm::vec2 project2D(glm::vec3 point)
    {
        point -= height;
        float dxp = point.x * b.y * normal.z + point.y * b.z * normal.x + point.z * b.x * normal.y;
        float dxn = point.z * b.y * normal.x + point.x * b.z * normal.y + point.y * b.x * normal.z;
        float dx = dxp - dxn;
        float dyp = a.x * point.y * normal.z + a.y * point.z * normal.x + a.z * point.x * normal.y;
        float dyn = a.z * point.y * normal.x + a.x * point.z * normal.y + a.y * point.x * normal.z;
        float dy = dyp - dyn;
        return { dx / d, dy / d };
    }

    glm::vec3 project(glm::vec3 point)
    {
        glm::vec2 project = project2D(point);
        return height + project.x * a + project.y * b;
    }

    float getSignedDistance(glm::vec3 point)
    {
        float dzp = a.x * b.y * point.z + a.y * b.z * point.x + a.z * b.x * point.y;
        float dzn = a.z * b.y * point.x + a.x * b.z * point.y + a.y * b.x * point.z;
        float dz = dzp - dzn;
        return dz / d;
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