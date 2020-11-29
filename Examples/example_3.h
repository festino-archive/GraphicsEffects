#pragma once

#include <stdlib.h>
#include <GL/glut.h>
#include <iostream>
#include <cmath>

namespace ex3
{
    using namespace std;

    int win_width = 800;
    int win_height = 600;
    bool paused = false;
    void keyboard(unsigned char key, int x, int y)
    {
        if (key == 27) {
            if (paused) {
                paused = false;
                cout << "unpaused" << endl;
            }
            else {
                paused = true;
                cout << "paused" << endl;
            }
        }
        if (key == 'w')
            cout << "forward" << endl;
        if (key == ' ')
            cout << "jump" << endl;
    }

    void mouseMove(int mx, int my) {
        if (!paused) {
            int dx = mx - win_width / 2;
            int dy = my - win_height / 2;
            //cout << "mouse " << dx << " " << dy << endl;
            glutWarpPointer(win_width / 2, win_height / 2);
        }
    }
    void display()
    {
        /*glClear(GL_COLOR_BUFFER_BIT);
        glColor3f(1.0, 1.0, 1.0);

        glBegin(GL_QUADS);
        glColor3f(0.0, 0.0, 0.0);

        glVertex3f(0.1, 0.1, 0.0);
        glVertex3f(0.9, 0.1, 0.0);
        glVertex3f(0.9, 0.9, 0.0);
        glVertex3f(0.1, 0.9, 0.0);

        int num_segments = 5;
        float cx = win_width / 2;
        float cy = win_height / 2;
        float r = 10;

        glBegin(GL_LINE_LOOP);
        for (int ii = 0; ii < num_segments; ii++)
        {
            float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle

            float x = r * cosf(theta);//calculate the x component
            float y = r * sinf(theta);//calculate the y component

            glVertex2f(x + cx, y + cy);//output vertex

        }*/
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glTranslatef(0, 0, -10);

        double x, y;
        double cx = win_width / 300.0;
        double cy = win_height / 300.0;
        double r = 0.1;
        int num_segments = 20;
        glColor3d(0.4, 0.2, 0.2);

        glBegin(GL_LINE_LOOP);
        for (int ii = 0; ii < num_segments; ii++) {
            float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle 
            float x = r * cosf(theta);//calculate the x component 
            float y = r * sinf(theta);//calculate the y component 
            glVertex2f(x + cx, y + cy);//output vertex 
        }
        glEnd();

        glutSwapBuffers();
    }
    void GL_Setup(int width, int height) {
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);

        glViewport(0, 0, width, height);
        glMatrixMode(GL_PROJECTION);
        glEnable(GL_DEPTH_TEST);
        gluPerspective(45, (float)width / height, .1, 100);
        glMatrixMode(GL_MODELVIEW);
    }
    void reshape(int width, int height)
    {
        cout << "reshape" << endl;
        win_width = width;
        win_height = height;
        GL_Setup(win_width, win_height);
        display();
    }

    int start_example(int argc, char** argv) {
        glutInit(&argc, argv);
        glutInitWindowSize(win_width, win_height);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
        glutCreateWindow("Freetype OpenGL");
        glutReshapeFunc(reshape);
        glutDisplayFunc(display);
        glutIdleFunc(display);
        glutKeyboardFunc(keyboard);
        glutPassiveMotionFunc(mouseMove);
        GL_Setup(win_width, win_height);
        glutMainLoop();
        return 0;
    };
}