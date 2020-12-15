#pragma once

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class Billboard
{
	static GLuint textureLoc, alphaLoc, zLoc, center_posLoc, half_sizesLoc, offsetLoc, scaleLoc;
	static float aspect_ratio;

	GLuint textureID;
	glm::vec4 pos;
	glm::vec2 offset, scale;
	float z = 0;
	float real_z = 0;
	float alpha = 0.5;
	glm::vec4 ndc = glm::vec4();

	Vertex vertices[4];
	GLuint vertexBuffer;
	GLuint vertexArray;

public:
	Billboard(GLuint textureID, glm::vec3 center, glm::vec2 offset, glm::vec2 scale)
		: textureID(textureID), pos(glm::vec4(center, 1.0)), offset(offset), scale(scale)
	{
		vertices[0] = { { -1, -1, 0 }, { 0, 0 } };
		vertices[1] = { { -1, 1, 0 }, { 0, 1 } };
		vertices[2] = { { 1, 1, 0 }, { 1, 1 } };
		vertices[3] = { { 1, -1, 0 }, { 1, 0 } };

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(vertices[0]), vertices, GL_DYNAMIC_DRAW);

		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texcoords)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
	}

	void static setUniformLocations(GLuint program_billboard)
	{
		textureLoc = glGetUniformLocation(program_billboard, "billboardTexture");
		alphaLoc = glGetUniformLocation(program_billboard, "alpha");
		zLoc = glGetUniformLocation(program_billboard, "z");
		center_posLoc = glGetUniformLocation(program_billboard, "center_pos");
		half_sizesLoc = glGetUniformLocation(program_billboard, "half_sizes");
		offsetLoc = glGetUniformLocation(program_billboard, "offset");
		scaleLoc = glGetUniformLocation(program_billboard, "scale");
	}

	void static setAspectRatio(float ratio)
	{
		aspect_ratio = ratio;
	}

	void calcZ(glm::mat4x4 mvp, glm::mat4x4 mv)
	{
		ndc = mvp * pos;
		ndc /= ndc.w;
		z = ndc.z;
		real_z = -(mv * pos).z;
	}
	float getZ()
	{
		return z;
	}

	void setAlpha(float alpha)
	{
		this->alpha = alpha;
	}

	void draw()
	{
		if (z > 0 && z < 1)
		{
			glUniform1f(alphaLoc, alpha);
			glUniform1f(zLoc, z);
			glUniform2f(center_posLoc, ndc.x, ndc.y);
			float half_width = 1.0f / real_z;
			float half_height = aspect_ratio / real_z;
			glUniform2f(half_sizesLoc, half_width, half_height);
			glUniform2f(offsetLoc, offset.x, offset.y);
			glUniform2f(scaleLoc, scale.x, scale.y);

			glUniform1i(textureLoc, textureID);
			glActiveTexture(GL_TEXTURE0 + textureID);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glBindVertexArray(vertexArray);
			glDrawArrays(GL_QUADS, 0, 4);
		}
	}
};
GLuint Billboard::textureLoc, Billboard::alphaLoc, Billboard::zLoc, Billboard::center_posLoc, Billboard::half_sizesLoc, Billboard::offsetLoc, Billboard::scaleLoc;
float Billboard::aspect_ratio;