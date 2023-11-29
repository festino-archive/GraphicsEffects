#pragma once

#include<GL/glew.h>
#include <fstream>
#include <iostream>
#include "Camera.h"
#include "FileUtils.h"

using namespace std;

class Skybox
{
	GLuint skyboxMapLoc, skyboxMapID = 0;
	GLuint program;
    GLuint mvp_centeredLoc, cameraLoc;
    GLuint mvp_centered_prevLoc;

	GLuint vertexBuffer;
	GLuint vertexArray;
public:
	Skybox()
	{

	}

	void init()
	{
		FileUtils::loadShaders(program, "skybox.vert", "skybox.frag");
		glUseProgram(program);
        mvp_centeredLoc = glGetUniformLocation(program, "mvp_centered");
        mvp_centered_prevLoc = glGetUniformLocation(program, "mvp_centered_prev");
        cameraLoc = glGetUniformLocation(program, "camera");
        skyboxMapLoc = glGetUniformLocation(program, "skyboxMap");
        FileUtils::loadSkyboxTexture(skyboxMapID, "panorama_3.png", "panorama_1.png", "panorama_4.png", "panorama_5.png", "panorama_2.png", "panorama_0.png");
        glUniform1i(skyboxMapLoc, skyboxMapID);

        float skyboxVertices[] = {
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

        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	void draw(glm::vec3 camera_pos, float* mvp_centered_loc, float* mvp_centered_prev_loc)
	{
		glUseProgram(program);
        glUniform3fv(cameraLoc, 1, &camera_pos[0]);
        glUniformMatrix4fv(mvp_centeredLoc, 1, GL_FALSE, mvp_centered_loc);
        glUniformMatrix4fv(mvp_centered_prevLoc, 1, GL_FALSE, mvp_centered_prev_loc);
		glActiveTexture(GL_TEXTURE0 + skyboxMapID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxMapID);
        glBindVertexArray(vertexArray);
        glDrawArrays(GL_TRIANGLES, 0, 36);
	}
};