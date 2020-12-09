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
    GLuint mvpLoc, cameraLoc;

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
        FileUtils::loadSkyboxTexture(skyboxMapID, "panorama_3.png", "panorama_1.png", "panorama_4.png", "panorama_5.png", "panorama_2.png", "panorama_0.png");
        skyboxMapLoc = glGetUniformLocation(program, "skyboxMap");
        mvpLoc = glGetUniformLocation(program, "mvp_centered");
        cameraLoc = glGetUniformLocation(program, "camera");

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
        //for (int i = 0; i < 36*3; i++) {
        //    skyboxVertices[i] *= 10;
        //}

        glUniform1i(skyboxMapLoc, skyboxMapID);
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	void draw(Camera* camera)
	{
		glUseProgram(program);
        //cout << program << " -> " << next_program << endl;
        glm::vec3 pos = camera->getPosition();
        glUniform3fv(cameraLoc, 1, &pos[0]);
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, camera->getMvp_CenteredLoc());
		glActiveTexture(GL_TEXTURE0 + skyboxMapID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxMapID);
        glBindVertexArray(vertexArray);
        glDrawArrays(GL_TRIANGLES, 0, 36);
	}
};