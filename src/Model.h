#pragma once

#include <GL/glew.h>
#include "Vertex.h"
#include <iostream>

class Model
{
public:
    int vertices_count;
    Vertex* vertices;
    int indices_count;
    GLuint* indices;

    GLuint vertexBuffer;
    GLuint vertexArray;

    Model()
    {
        vertices_count = 8;
        vertices = new Vertex[vertices_count];
        glm::vec3 poses[] = {
            { -0.5f, -0.5f, -0.5f },
            { -0.5f, 0.5f, -0.5f },
            { 0.5f, 0.5f, -0.5f },
            { 0.5f, -0.5f, -0.5f },
            { -0.5f, -0.5f, 0.5f },
            { -0.5f, 0.5f, 0.5f },
            { 0.5f, 0.5f, 0.5f },
            { 0.5f, -0.5f, 0.5f }
        };
        for (int i = 0; i < vertices_count; i++)
            vertices[i] = { poses[i], {poses[i].x + 0.5f, poses[i].y + 0.5f} };

        indices_count = 6 * 4;
        indices = new GLuint[indices_count];
        GLuint t2[] = { 3, 2, 1, 0,
            4, 5, 6, 7,
            0, 1, 5, 4,
            1, 2, 6, 5,
            2, 3, 7, 6,
            3, 0, 4, 7 };
        for (int i = 0; i < indices_count; i++)
            indices[i] = t2[i];

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
        //std::cout << "draw" << std::endl;
        glBindVertexArray(vertexArray);
        glDrawElements(GL_QUADS, indices_count, GL_UNSIGNED_INT, indices);
    }
};