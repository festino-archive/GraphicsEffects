
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <cmath>
#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <vector>
#include "Camera.h"

using namespace std;

int win_width = 800;
int win_height = 600;
GLuint program;

bool paused = false;
float sensitivity_hor = 0.0012; // TODO: make pixel independent
float sensitivity_vert = 0.0015;
Camera cam = Camera(0, 0, glm::vec3(), 0, 0);
GLint mvpLoc;

constexpr float MICROSEC = 1.0f / 1000000;
constexpr int TIMES_COUNT = 10;
vector<float> times(TIMES_COUNT, 0.0f);
int times_index = 0;
chrono::steady_clock::time_point prevFrame;
bool initFrame = true;

bool holdW = false, holdA = false, holdS = false, holdD = false;
bool holdSpace = false, holdShift = false;
glm::vec3 speed;

#pragma pack(1)
struct Vertex
{
    float x, y, z;
};
#pragma pack()

Vertex cube_vertices[] = {
    { -0.5f, -0.5f, -0.5f },
    { -0.5f, 0.5f, -0.5f },
    { 0.5f, 0.5f, -0.5f },
    { 0.5f, -0.5f, -0.5f },
    { -0.5f, -0.5f, 0.5f },
    { -0.5f, 0.5f, 0.5f },
    { 0.5f, 0.5f, 0.5f },
    { 0.5f, -0.5f, 0.5f }
};

GLuint cube_indices[] = {
    3,2,1,0,
    4,5,6,7,
    0, 1, 5, 4,
    1, 2, 6, 5,
    2, 3, 7, 6,
    3, 0, 4, 7
};

GLuint vertexBuffer;
GLuint vertexArray;

void keyState(unsigned char key, int x, int y, bool down)
{
    key = tolower(key);
    if (key == 'w')
        holdW = down;
    if (key == 's')
        holdS = down;
    if (key == 'a')
        holdA = down;
    if (key == 'd')
        holdD = down;
    if (key == ' ')
        holdSpace = down;
    if (key == 112)
        holdShift = !holdShift;
    //holdShift = glutGetModifiers() & GLUT_ACTIVE_SHIFT;
}
void setPaused(bool val)
{
    paused = val;
    if (paused) {
        glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
    } else {
        glutSetCursor(GLUT_CURSOR_NONE);
    }
}
void keyDown(unsigned char key, int x, int y)
{
    if (key == 27)
        setPaused(!paused);
    keyState(key, x, y, true);
}
void keySpecial(int key, int x, int y)
{
    if (key == 27)
        setPaused(!paused);
    keyState(key, x, y, true);
}
void keyUp(unsigned char key, int x, int y)
{
    keyState(key, x, y, false);
}

void mouseMove(int mx, int my) {
    if (!paused) {
        int dx = mx - win_width / 2;
        int dy = my - win_height / 2;
        glutWarpPointer(win_width / 2, win_height / 2);
        cam.addAngle(dx * sensitivity_hor, dy * sensitivity_vert);
    }
}

void moveCamera(float time)
{
    speed = glm::vec3();
    if (holdW)
        speed.z += 1;
    if (holdS)
        speed.z -= 1;
    if (holdA)
        speed.x += 1;
    if (holdD)
        speed.x -= 1;
    if (holdSpace)
        speed.y -= 1;
    if (holdShift)
        speed.y += 1;

    speed *= time;
    cam.moveHor(speed.z, speed.x, speed.y);
}

void idle()
{
    chrono::steady_clock::time_point curFrame = chrono::steady_clock::now();
    if (initFrame) {
        initFrame = false;
    } else {
        double delta = double(chrono::duration_cast<chrono::microseconds>(curFrame - prevFrame).count());
        if (times.size() >= TIMES_COUNT)
            times[times_index] = delta;
        else
            times.push_back(delta);
        if (++times_index >= TIMES_COUNT)
            times_index = 0;

        double avg = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
        moveCamera(avg * MICROSEC);
        // animations
    }
    prevFrame = curFrame;
    glutPostRedisplay();
}

void display()
{
    cam.updateMvp();
    glUseProgramObjectARB(program);
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &cam.mvp[0][0]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawElements(GL_QUADS, sizeof(cube_indices) / sizeof(cube_indices[0]), GL_UNSIGNED_INT, cube_indices);
    glFlush();
    glutSwapBuffers();
}
void reshape(int width, int height)
{
    win_width = width;
    win_height = height;
    cam.updateProj(win_width, win_height);
    glViewport(0, 0, win_width, win_height);
    times.clear();
    times_index = 0;
    initFrame = true;
}

void initCamera()
{
    cam = Camera(0, 0, glm::vec3(0, 0, -2), win_width, win_height);

    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &cam.mvp[0][0]);
}

void initGL()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);

    std::ifstream vsh_file("scene_1.vert");
    std::ifstream fsh_file("scene_1.frag");
    string vsh_src = string(istreambuf_iterator<char>(vsh_file), istreambuf_iterator<char>());
    string fsh_src = string(istreambuf_iterator<char>(fsh_file), istreambuf_iterator<char>());

    program = glCreateProgram();
    GLenum vertex_shader = glCreateShader(GL_VERTEX_SHADER_ARB);
    GLenum fragment_shader = glCreateShader(GL_FRAGMENT_SHADER_ARB);

    const char *src = vsh_src.c_str();
    glShaderSource(vertex_shader, 1, &src, NULL);
    src = fsh_src.c_str();
    glShaderSource(fragment_shader, 1, &src, NULL);

    glCompileShader(vertex_shader);
    glCompileShader(fragment_shader);

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    glLinkProgram(program);
    char log[10000];
    int log_len;
    glGetProgramInfoLog(program, sizeof(log) / sizeof(log[0]) - 1, &log_len, log);
    log[log_len] = 0;
    printf("LOG: %s\n", log);

    mvpLoc = glGetUniformLocation(program, "mvp");
    glUseProgramObjectARB(program);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

int start_scene_1(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(win_width, win_height);
    glutCreateWindow("OpenGL");

    GLint GlewInitResult = glewInit();
    if (GLEW_OK != GlewInitResult)
    {
        printf("ERROR: %s", glewGetErrorString(GlewInitResult));
        exit(EXIT_FAILURE);
    }

    initGL();
    initCamera();
    setPaused(false);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(idle);

    glutSpecialFunc(keySpecial);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
    glutPassiveMotionFunc(mouseMove);

    glutMainLoop();
    return 0;
}