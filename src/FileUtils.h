#pragma once
#include<GL/glew.h>
#include<GL/freeglut.h>
#include<glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <fstream>
#include <iostream>
#include <iterator>

#include"FreeImage.h"

using namespace std;

class FileUtils
{
public:
	static bool loadShaders(GLuint &program, const char* vert, const char* frag)
	{
		std::ifstream vsh_file(vert);
		string vsh_src = string(istreambuf_iterator<char>(vsh_file), istreambuf_iterator<char>());
		GLenum vertex_shader = glCreateShader(GL_VERTEX_SHADER_ARB);
		const char* src = vsh_src.c_str();
		glShaderSource(vertex_shader, 1, &src, NULL);
		glCompileShader(vertex_shader);

		std::ifstream fsh_file(frag);
		string fsh_src = string(istreambuf_iterator<char>(fsh_file), istreambuf_iterator<char>());
		GLenum fragment_shader = glCreateShader(GL_FRAGMENT_SHADER_ARB);
		src = fsh_src.c_str();
		glShaderSource(fragment_shader, 1, &src, NULL);
		glCompileShader(fragment_shader);

		program = glCreateProgram();
		glAttachShader(program, vertex_shader);
		glAttachShader(program, fragment_shader);
		glLinkProgram(program);

		char log[10000];
		int log_len;
		glGetProgramInfoLog(program, sizeof(log) / sizeof(log[0]) - 1, &log_len, log);
		log[log_len] = 0;
		if (log_len == 0) {
			log[0] = 'O';
			log[1] = 'K';
			log[2] = '\0';
		}
		printf("Log: %s\n", log);

		return log_len == 0;
	}

	static void loadTextures(GLuint& textureID, const char fname[])
	{
		FREE_IMAGE_FORMAT format = FreeImage_GetFileType(fname, 0);
		FIBITMAP* image = FreeImage_Load(format, fname);
		FIBITMAP* temp = image;
		image = FreeImage_ConvertTo32Bits(image);
		FreeImage_Unload(temp);

		int w = FreeImage_GetWidth(image);
		int h = FreeImage_GetHeight(image);

		GLubyte* bits = (GLubyte*)FreeImage_GetBits(image);

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid*)bits);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		FreeImage_Unload(image);
	}

	static void loadSkyboxTexture(GLuint& skyboxTexID, const char xn[], const char xp[], const char yn[], const char yp[], const char zn[], const char zp[])
	{
		glGenTextures(1, &skyboxTexID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexID);

		const char *filenames[6] = { xn, xp, yn, yp, zn, zp };
		GLenum directions[6] = {
				GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_X,
				GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
				GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, GL_TEXTURE_CUBE_MAP_POSITIVE_Z
		};
		for (int i = 0; i < 6; i++)
		{
			FREE_IMAGE_FORMAT format = FreeImage_GetFileType(filenames[i], 0);
			FIBITMAP* image = FreeImage_Load(format, filenames[i]);
			FIBITMAP* temp = image;
			image = FreeImage_ConvertTo32Bits(image);
			FreeImage_Unload(temp);

			int width = FreeImage_GetWidth(image);
			int height = FreeImage_GetHeight(image);
			GLubyte* bits = (GLubyte*)FreeImage_GetBits(image);
			glTexImage2D(directions[i], 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid*)bits);
			FreeImage_Unload(image);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
};