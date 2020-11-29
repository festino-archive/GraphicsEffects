#pragma once

#include <stdlib.h>
#include <GL/glut.h>

namespace ex2
{
    void NIAppIdle(void) {

    }
    void NIAppDisplay(void) {

    }

    int start_example(int argc, char* argv[])
    {
        // Setup windwing
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
        glutInitWindowSize(800, 600);
        glutCreateWindow("HELLO WORLD");

        // Define GLUT callbacks
        glutIdleFunc(NIAppIdle);
        glutDisplayFunc(NIAppDisplay);

        // Enter render loop
        glutMainLoop();
        return 0;
    }
}