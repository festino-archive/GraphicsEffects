#pragma once

#include "Model.h"
#include "Plane.h"

class Mirror
{
public:
    Model *model;
    Plane* plane;
    glm::vec3 pos_prev;
    glm::mat4x4 rot_prev;

    Mirror(Model *model)
    {
        this->model = model;
        updatePlane();
    }

    void updatePlane()
    {
        delete plane;
        plane = new Plane(model->getWorldPoint(0), model->getWorldPoint(1), model->getWorldPoint(2));
    }

    void setPrev(glm::mat4x4 rot, glm::vec3 pos)
    {
        rot_prev = rot;
        pos_prev = pos;
    }

    glm::vec3 flip(glm::vec3 pos)
    {
        return plane->flip(pos);
    }
    glm::mat4x4 flipRot(glm::mat4x4 rot)
    {
        return plane->flipRotation(rot);
    }
    std::array<glm::vec3, 3> getPlane()
    {
        return { model->getWorldPoint(0), model->getWorldPoint(1), model->getWorldPoint(2) };
    }
};