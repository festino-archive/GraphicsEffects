#pragma once

#include <GL/glew.h>
#include "FileUtils.h"

class Texture
{
	GLuint colorMapLoc, normalMapLoc;
	GLuint colorMapID, normalMapID;
	GLuint linearFiltering;

    GLuint light_impactLoc, spec_impactLoc, shininessLoc;

    float light_impact = 0.7;
    float spec_impact = 0.2;
    float shininess = 1;
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

        light_impactLoc = glGetUniformLocation(program, "light_impact");
        spec_impactLoc = glGetUniformLocation(program, "spec_impact");
        shininessLoc = glGetUniformLocation(program, "shininess");
	}

    void setProperties(float light_impact, float spec_impact, float shininess)
    {
        this->light_impact = light_impact;
        this->spec_impact = spec_impact;
        this->shininess = shininess;
    }

    void use()
    {
        glUniform1f(light_impactLoc, light_impact);
        glUniform1f(spec_impactLoc, spec_impact);
        glUniform1f(shininessLoc, shininess);

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