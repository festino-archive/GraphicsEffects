#include <stdlib.h>
#include <GL/glut.h>

unsigned int CUBE_SIZE = 3 * 8;

float CUBE_POINTS[] = {
	// 1
	-1, -1, -1,
	1, -1, -1,
	1, 1, -1,

	-1, -1, -1,
	-1, 1, -1,
	1, 1, -1,
	// 2
	-1, -1, -1,
	-1, 1, -1,
	-1, 1, 1,

	-1, -1, -1,
	-1, -1, 1,
	-1, 1, 1,
	// 3
	-1, -1, -1,
	1, -1, -1,
	1, -1, 1,

	-1, -1, -1,
	-1, -1, 1,
	1, -1, 1,
};

int
start_scene_1(int argc, char** argv)
{
	glutInit(&argc, argv);
	return 0;
}