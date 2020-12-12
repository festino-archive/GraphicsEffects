#pragma once

#include <GL/glew.h>
#include <iostream>
#include "Vertex.h"
#include "Texture.h"

class TexturedModel
{
public:
    int vertices_count;
    Vertex* vertices;

    GLuint vertexBuffer;
    GLuint vertexArray;

    Texture* texture;

    TexturedModel(int vertices_count, Vertex* vertices, Texture* texture)
    {
        this->vertices_count = vertices_count;
        this->vertices = vertices;
        this->texture = texture;

        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, vertices_count * sizeof(*vertices), vertices, GL_STATIC_DRAW);

        glGenVertexArrays(1, &vertexArray);
        glBindVertexArray(vertexArray);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texcoords)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
    }

    void draw()
    {
        if (texture != nullptr)
            texture->use();
        glBindVertexArray(vertexArray);
        glDrawArrays(GL_TRIANGLES, 0, vertices_count);
    }
};