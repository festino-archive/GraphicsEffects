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

    void draw()
    {
        texture->use();
        model->draw();
    }
};