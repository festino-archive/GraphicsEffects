#pragma once

#include<GL/glew.h>
#include <fstream>
#include <iostream>
#include "FileUtils.h"

using namespace std;

class Skybox
{
	GLuint skyboxMapLoc, skyboxMapID;
	GLuint program;
public:
	Skybox()
	{

	}

	void init()
	{
		FileUtils::loadShaders(program, "skybox.vert", "skybox.frag");
		skyboxMapLoc = glGetUniformLocation(program, "skyboxMap");
		FileUtils::loadSkyboxTexture(skyboxMapID, "panorama0.png", "panorama1.png", "panorama2.png", "panorama3.png", "panorama4.png", "panorama5.png");
		glUseProgram(program);
		glUniform1i(skyboxMapLoc, skyboxMapID);
	}

	void draw(GLuint next_program)
	{
		glUseProgram(program);
		glActiveTexture(GL_TEXTURE0 + skyboxMapID);

		glUseProgram(next_program);
	}
};