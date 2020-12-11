#pragma once

#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "FileUtils.h"

namespace ex6
{
    int win_width = 1080;
    int win_height = 720;

    GLuint program;
    GLuint depthShader;

    GLuint ssaoFramebufferID;
    GLuint ssaoDepthTextureID;

    void DrawScene() {

    }

    void main_loop_function() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFramebufferID);
        glViewport(0, 0, 1024, 1024);

        glUseProgram(depthShader);
        GLuint depthLoc = glGetUniformLocation(depthShader, "depthMVP");
        glm::mat4x4 view = glm::rotate(0.0f, glm::vec3(0, 1, 0));
        glm::mat4x4 mvp = view * glm::perspective(60.0f, win_width / (float)win_height, 0.1f, 10.0f);
        glUniformMatrix4fv(depthLoc, 1, GL_FALSE, &mvp[0][0]);

        DrawScene();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, win_width, win_height);
        glutSwapBuffers();
    }
    void GL_Setup(int width, int height) {
        FileUtils::loadShaders(program, "ex6.vert", "ex6.frag");
        FileUtils::loadShaders(depthShader, "ex6_depth.vert", "ex6_depth.frag");

        glViewport(0, 0, width, height);
        glMatrixMode(GL_PROJECTION);
        glEnable(GL_DEPTH_TEST);
        gluPerspective(45, (float)width / height, .1, 100);
        glMatrixMode(GL_MODELVIEW);

        glGenFramebuffers(1, &ssaoFramebufferID);
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFramebufferID);

        glGenTextures(1, &ssaoDepthTextureID);
        glBindTexture(GL_TEXTURE_2D, ssaoDepthTextureID);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, ssaoDepthTextureID, 0);

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    int start_example(int argc, char** argv) {
        glutInit(&argc, argv);
        glutInitWindowSize(win_width, win_height);
        glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
        glutCreateWindow("GLUT Example!!!");
        glutIdleFunc(main_loop_function);
        glutDisplayFunc(main_loop_function);
        GL_Setup(win_width, win_height);

        glutMainLoop();
        return 0;
    }
}