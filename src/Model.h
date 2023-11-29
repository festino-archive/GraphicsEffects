#pragma once

#include <GL/glew.h>
#include <iostream>
#include "Vertex.h"

class Model
{
    void setScaleNoRecalc(glm::vec3 scale)
    {
        this->scale = glm::identity<glm::mat4x4>();
        this->scale[0][0] = scale[0];
        this->scale[1][1] = scale[1];
        this->scale[2][2] = scale[2];
        updateTransformation();
    }
    void updateTransformation()
    {
        modelToWorld = glm::translate(shift) * rotation * scale;
    }
public:
    static GLuint modelToWorldLoc;
    static GLuint modelToWorld_prevLoc;

    int vertices_count;
    Vertex* vertices;

    GLuint vertexBuffer;
    GLuint vertexArray;

    // from local
    glm::vec3 shift;
    glm::mat4x4 rotation;
    glm::mat4x4 scale;
    glm::mat4x4 modelToWorld;
    glm::mat4x4 modelToWorld_prev;

    Model(int vertices_count, Vertex* vertices)
        : shift(glm::vec3()), rotation(glm::identity<glm::mat4x4>()), modelToWorld(glm::identity<glm::mat4x4>()), scale(glm::identity<glm::mat4x4>())
    {

        this->vertices_count = vertices_count;
        this->vertices = vertices;

        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, vertices_count * sizeof(*vertices), vertices, GL_STATIC_DRAW);

        glGenVertexArrays(1, &vertexArray);
        glBindVertexArray(vertexArray);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texcoords)));
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, tangent)));
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, bitangent)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
    }

    void nextFrame()
    {
        modelToWorld_prev = modelToWorld;
    }

    void setShift(glm::vec3 shift)
    {
        this->shift = shift;
        updateTransformation();
    }

    void setRotation(glm::mat4x4 rotation)
    {
        this->rotation = rotation;
        updateTransformation();
    }

    void setScale(glm::vec3 scale)
    {
        setScaleNoRecalc(scale);
        updateTransformation();
    }

    void setTransform(glm::vec3 shift, glm::mat4x4 rotation)
    {
        this->shift = shift;
        this->rotation = rotation;
        updateTransformation();
    }

    void setTransform(glm::vec3 shift, glm::mat4x4 rotation, glm::vec3 scale)
    {
        this->shift = shift;
        this->rotation = rotation;
        setScaleNoRecalc(scale);
        updateTransformation();
    }

    glm::vec3 getWorldPoint(int index)
    {
        return modelToWorld * glm::vec4(vertices[index].position, 1.0);
    }

    glm::mat4x4 getTransform()
    {
        return modelToWorld;
    }

    void draw()
    {
        glUniformMatrix4fv(modelToWorldLoc, 1, GL_FALSE, &modelToWorld[0][0]);
        glUniformMatrix4fv(modelToWorld_prevLoc, 1, GL_FALSE, &modelToWorld_prev[0][0]);
        glBindVertexArray(vertexArray);
        glDrawArrays(GL_TRIANGLES, 0, vertices_count);
    }

    static void setUniformLocations(GLuint program)
    {
        modelToWorldLoc = glGetUniformLocation(program, "modelToWorld");
        modelToWorld_prevLoc = glGetUniformLocation(program, "modelToWorld_prev");
    }
};

GLuint Model::modelToWorldLoc, Model::modelToWorld_prevLoc;