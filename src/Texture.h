#pragma once

#include <GL/glew.h>
#include "FileUtils.h"

class Texture
{
	GLuint colorMapLoc, normalMapLoc;
	GLuint colorMapID, normalMapID;
	GLuint linearFiltering;
public:
	Texture(GLuint program, const char colorPath[], const char normalPath[])
	{
        glGenSamplers(1, &linearFiltering);
        glSamplerParameteri(linearFiltering, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//_MIPMAP_LINEAR);
        glSamplerParameteri(linearFiltering, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//_MIPMAP_LINEAR);

        colorMapLoc = glGetUniformLocation(program, "colorMap");
        normalMapLoc = glGetUniformLocation(program, "normalMap");
        FileUtils::loadTextures(colorMapID, colorPath);
        FileUtils::loadTextures(normalMapID, normalPath);
	}

    void use()
    {
        glUniform1i(colorMapLoc, colorMapID);
        glUniform1i(normalMapLoc, normalMapID);
        glActiveTexture(GL_TEXTURE0 + colorMapID);
        glBindTexture(GL_TEXTURE_2D, colorMapID);
        glBindSampler(colorMapID, linearFiltering);
        glActiveTexture(GL_TEXTURE0 + normalMapID);
        glBindTexture(GL_TEXTURE_2D, normalMapID);
        glBindSampler(normalMapID, linearFiltering);
    }
};