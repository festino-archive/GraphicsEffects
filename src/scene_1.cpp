
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <cmath>
#include <chrono>
#include <numeric>
#include <vector>
#include <algorithm>
#include "controls.h"
#include "Camera.h"
#include "Model.h"
#include "Plane.h"
#include "Utils.h"
#include "Omnilight.h"
#include "Skybox.h"

using namespace std;

int win_width = 1280;
int win_height = 720;
GLuint program;
GLuint program_depth_nuller;
GLuint nuller_zLoc;
GLuint program_2d;
GLuint program_write_min_z;
GLuint color_maskLoc, white_textureLoc, white_textureID, linearFiltering;

GLuint mvpLoc, cameraLoc, min_z_mvpLoc;
GLuint lightsLoc;
GLuint min_z_fbo, min_z_texture, min_zLoc;
//GLenum DrawBuffers[1] = { GL_DEPTH_ATTACHMENT };

Skybox skybox;
vector<Omnilight*> lights = vector<Omnilight*>();
Omnilight* movable_light;
vector<Model*> models = vector<Model*>();
vector<Model*> mirror_faces = vector<Model*>();

Camera camera = Camera(0, 0, glm::vec3(), 0, 0);
float init_yaw = 45, init_pitch = 0;
glm::vec3 init_pos = glm::vec3(-19, 1, 18);
float fov = 45.0f, near_dist = 0.1f, far_dist = 50.0f;

constexpr float MICROSEC = 1.0f / 1000000;
constexpr int TIMES_COUNT = 10;
float full_time = 0;
vector<float> times(TIMES_COUNT, 0.0f);
int times_index = 0;
chrono::steady_clock::time_point prevFrame;
bool initFrame = true;

Controller *controller;

void keyDown(unsigned char key, int x, int y)
{
    controller->keyDown(key, x, y);
}
void keySpecial(int key, int x, int y)
{
    controller->keySpecial(key, x, y);
}
void keyUp(unsigned char key, int x, int y)
{
    controller->keyUp(key, x, y);
}
void mouseMove(int mx, int my) {
    controller->mouseMove(mx, my);
}

void loadModels()
{
    Texture* texture = new Texture(program, "prev.png", "smooth_normal.png");
    Model* model = makeStaticCube(1, { 0, 0, -1.5 }, glm::identity<glm::mat4>(), texture);
    models.push_back(model);
    texture = new Texture(program, "brick.png", "brick_normal.png");
    model = makeStaticCube(2, { 3, 0.5, 0 }, glm::identity<glm::mat4>(), texture);
    models.push_back(model);

    float size = 1;
    float offset = -3.5;
    for (int i = 0; i < 10; i++) {
        size *= 0.9;
        model = makeStaticCube(size, { 0, 0, offset }, glm::identity<glm::mat4>(), texture);
        offset -= 1 + size;
        models.push_back(model);
    }

    texture = new Texture(program, "prev.png", "smooth_normal.png");
    glm::vec3 p1 = { -3.5, -1.5, 1.5 };
    glm::vec3 p3 = { -3.5, 1.5, 1.5 };
    glm::vec3 p2 = { 0.5, -1.5, 3.5 };
    glm::vec3 p4 = { 0.5, 1.5, 3.5 };
    Vertex* vertices = new Vertex[6];
    vertices[0] = { p1, {0, 0} };
    vertices[1] = { p2, {0, 0} };
    vertices[2] = { p4, {0, 0} };
    vertices[3] = { p1, {0, 0} };
    vertices[4] = { p3, {0, 0} };
    vertices[5] = { p4, {0, 0} };
    model = new Model(6, vertices, texture);
    mirror_faces.push_back(model);
}

void idle()
{
    chrono::steady_clock::time_point curFrame = chrono::steady_clock::now();
    if (initFrame) {
        initFrame = false;
    } else {
        double delta = double(chrono::duration_cast<chrono::microseconds>(curFrame - prevFrame).count());
        delta *= MICROSEC;
        full_time += delta;
        if (times.size() >= TIMES_COUNT)
            times[times_index] = delta;
        else
            times.push_back(delta);
        if (++times_index >= TIMES_COUNT)
            times_index = 0;

        double avg = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
        //cout << "FPS: " << 1 / avg << endl;

        // animations
        controller->moveCamera(avg);
        float angle = full_time / 20;
        movable_light->light_pos = glm::vec4(2 * glm::sin(angle), 1, 2 * glm::cos(angle), 0);
    }
    prevFrame = curFrame;
    glutPostRedisplay();
}

void renderRegularObjects()
{
    for (Model* model : models)
    {
        model->draw();
    }
}

void renderHUD()
{
    glUseProgram(program_2d);

    glUniform1i(white_textureLoc, white_textureID);
    glActiveTexture(GL_TEXTURE0 + white_textureID);
    glBindTexture(GL_TEXTURE_2D, white_textureID);
    glBindSampler(white_textureID, linearFiltering);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, win_width, win_height, 0.0, -1.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    //glPushMatrix();        ----Not sure if I need this
    glLoadIdentity();
    glDisable(GL_CULL_FACE);
    glClear(GL_DEPTH_BUFFER_BIT);

    float ratio = win_width / (float)win_height;
    double corner = 1.0;
    double offset_hor = 0.08;
    double cx = corner - offset_hor;
    double cy = corner - offset_hor * ratio;
    double r = 0.05;
    int num_segments = 80;

    glEnable(GL_LINE_SMOOTH);
    glBegin(GL_LINE_LOOP);
    glLineWidth(1.0f);
    //glColor3f(0.7f, 0.7f, 0.7f);
    glUniform4f(color_maskLoc, 0.7f, 0.7f, 0.7f, 1.0f);
    for (int ii = 0; ii < num_segments; ii++) {
        float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);
        float x = r * cosf(theta);
        float y = r * sinf(theta) * ratio;
        glVertex2f(x + cx, y + cy);
    }
    glEnd();
    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.0f, 0.0f);
    glUniform4f(color_maskLoc, 0.0f, 0.0f, 0.0f, 1.0f);
    float marker_width2 = r / 20;
    float marker_length = r / 2;
    float x_left = - marker_width2;
    float x_right =  + marker_width2;
    float y_bottom = r - marker_length;
    float y_top = r;
    glm::vec2 lb(x_left, y_bottom), rb(x_right, y_bottom), lt(x_left, y_top), rt(x_right, y_top);
    float yaw = camera.getYaw();
    glm::mat2x2 rot = glm::mat2x2{ cos(yaw), -sin(yaw), sin(yaw), cos(yaw)};
    lb = rot * lb;
    rb = rot * rb;
    lt = rot * lt;
    rt = rot * rt;
    glVertex2f(cx + lb.x, cy + lb.y * ratio);
    glVertex2f(cx + rb.x, cy + rb.y * ratio);
    glVertex2f(cx + rt.x, cy + rt.y * ratio);
    glVertex2f(cx + lt.x, cy + lt.y * ratio);
    glEnd();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void clearDepthRespectsStencil(float depth)
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glUseProgram(program_depth_nuller);
    glUniform1f(nuller_zLoc, depth);
    
    glBegin(GL_QUADS);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(1.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();
    glUseProgram(program);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthFunc(GL_LEQUAL);
}

void clearMinZ()
{
    glBindFramebuffer(GL_FRAMEBUFFER, min_z_fbo);
    clearDepthRespectsStencil(0.0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void display()
{
    clearMinZ();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    camera.updateMvp();
    glUseProgram(program);
    glm::vec3 pos = camera.getPosition();
    glUniform3fv(cameraLoc, 1, &pos[0]);
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, camera.getMvpLoc());

    if (controller->lantern)
    {
        controller->lantern_obj->light_pos = glm::vec4(camera.getRelative(-0.5, 0, -0.3), 1);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightsLoc);
    Omnilight *lights_arr = new Omnilight[lights.size()];
    for (int i = 0; i < lights.size(); i++)
        lights_arr[i] = *lights[i];
    glBufferData(GL_SHADER_STORAGE_BUFFER, lights.size() * sizeof(Omnilight), &lights_arr[0], GL_STATIC_READ);
    delete[] lights_arr;
    //glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(lights), &lights[0]);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    renderRegularObjects();

    for (Model* mirror : mirror_faces)
    {
        glEnable(GL_STENCIL_TEST);

        // set up stencil
        glStencilOp(GL_ZERO, GL_ZERO, GL_REPLACE);
        glStencilFunc(GL_ALWAYS, 1, 1);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDepthMask(GL_FALSE);
        mirror->draw();
        glDepthMask(GL_TRUE);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        glStencilFunc(GL_EQUAL, 1, 1);

        glUseProgram(program_write_min_z);
        glUniformMatrix4fv(min_z_mvpLoc, 1, GL_FALSE, camera.getMvpLoc());
        glBindFramebuffer(GL_FRAMEBUFFER, min_z_fbo);
        glDepthFunc(GL_GREATER);
        glDisable(GL_ALPHA_TEST);
        mirror->draw();
        glEnable(GL_ALPHA_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        clearDepthRespectsStencil(1.0);
        min_zLoc = glGetUniformLocation(program, "min_z");
        glUniform1i(min_zLoc, min_z_texture);
        glActiveTexture(GL_TEXTURE0 + min_z_texture);
        glBindTexture(GL_TEXTURE_2D, min_z_texture);

        // render mirrored objects
        Plane plane = Plane(mirror->vertices[0].position, mirror->vertices[1].position, mirror->vertices[2].position);
        glm::vec3 pos_flipped = plane.flip(camera.getPosition());
        glm::mat4x4 mvp_flipped_centered = camera.flippedMvp_centered(plane);
        glm::mat4x4 mvp_flipped = mvp_flipped_centered * glm::translate(-pos_flipped);
        //cout << to_string(mvp_flipped * glm::vec4(pos_flipped, 1.0f)) << " " << to_string(mvp_flipped * glm::vec4(0.0, 0.0, 0.0, 1.0f)) << endl;
        glUniform3fv(cameraLoc, 1, &pos_flipped[0]);
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp_flipped[0][0]);
        renderRegularObjects();

        clearMinZ();

        // render mirrored skybox
        skybox.draw(pos_flipped, &mvp_flipped_centered[0][0]);

        glUseProgram(program);
        glUniform3fv(cameraLoc, 1, &pos[0]);
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, camera.getMvpLoc());
        // set correct z-buffer
        glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
        glStencilFunc(GL_ALWAYS, 0, 1);
        glDepthFunc(GL_ALWAYS);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        mirror->draw();
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDepthFunc(GL_LEQUAL);

        glDisable(GL_STENCIL_TEST);
    }

    skybox.draw(camera.getPosition(), camera.getMvp_CenteredLoc());

    if (controller->flag_info)
    {
        renderHUD();
    }

    glFlush();
    glutSwapBuffers();
}
void reshape(int width, int height)
{
    win_width = width;
    win_height = height;
    camera.updateProjSize(win_width, win_height);
    glViewport(0, 0, win_width, win_height);
    times.clear();
    times_index = 0;
    initFrame = true;
}

void initCamera()
{
    camera = Camera(init_yaw, init_pitch, init_pos, win_width, win_height);
    camera.setProjParams(fov, near_dist, far_dist);
}

void initFBO()
{
    FileUtils::loadShaders(program_write_min_z, "write_depth.vert", "write_depth.frag");
    min_z_mvpLoc = glGetUniformLocation(program_write_min_z, "mvp");
    glGenFramebuffers(1, &min_z_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, min_z_fbo);
    glGenTextures(1, &min_z_texture);
    glBindTexture(GL_TEXTURE_2D, min_z_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, win_width, win_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0); // update size
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_GEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, min_z_texture, 0);
    //glDrawBuffers(1, DrawBuffers);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Min z framebuffer error" << std::endl;

    min_zLoc = glGetUniformLocation(program, "min_z");
    glUniform1i(min_zLoc, min_z_texture);
}

void initGL()
{
    skybox = Skybox();
    skybox.init();

    FileUtils::loadShaders(program_depth_nuller, "nuller.vert", "nuller.frag");
    nuller_zLoc = glGetUniformLocation(program_depth_nuller, "z");

    FileUtils::loadShaders(program_2d, "2d.vert", "2d.frag");
    glUseProgram(program_2d);
    white_textureLoc = glGetUniformLocation(program_2d, "guiTexture");
    color_maskLoc = glGetUniformLocation(program_2d, "color_mask");
    FileUtils::loadTextures(white_textureID, "white.png");
    glGenSamplers(1, &linearFiltering);
    glSamplerParameteri(linearFiltering, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glSamplerParameteri(linearFiltering, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    FileUtils::loadShaders(program, "main.vert", "main.frag");
    glUseProgram(program);
    mvpLoc = glGetUniformLocation(program, "mvp");
    cameraLoc = glGetUniformLocation(program, "camera");

    glGenBuffers(1, &lightsLoc);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightsLoc);
    glBufferData(GL_SHADER_STORAGE_BUFFER, lights.size() * sizeof(Omnilight), &lights[0], GL_STATIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightsLoc);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    lights.push_back(new Omnilight { {1, 0.5, 1, 0}, {0.9, 1, 0.9, 0}, {0.9, 1, 0.9, 0}, 0.7, 0.2 });
    movable_light = new Omnilight { {0, 1, 2, 0}, {0.9, 0, 0, 0}, {0.9, 0, 0, 0}, 0.7, 0.2 };
    lights.push_back(movable_light);

    initFBO();
    loadModels();

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // GL_LINE
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

int start_scene_1(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
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
    controller = new Controller(&win_width, &win_height, &camera, &lights);
    glutSpecialFunc(keySpecial);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutPassiveMotionFunc(mouseMove);
    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    glutIdleFunc(idle);

    glutMainLoop();
    return 0;
}