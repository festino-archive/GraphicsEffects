
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
#include "Plane.h"
#include "Skybox.h"
#include "Utils.h"
#include "Omnilight.h"
#include "Model.h"
#include "TexturedModel.h"
#include "PortalPair.h"

using namespace std;

int win_width = 1280;
int win_height = 720;
GLuint program;
GLuint program_depth_nuller;
GLuint nuller_zLoc;
GLuint program_2d;
GLuint program_write_min_z;
GLuint color_maskLoc, white_textureLoc, white_textureID, linearFiltering;

GLuint mvpLoc, cameraLoc;
GLuint lightsLoc;

GLuint min_z_fbo, min_z_texture, min_zLoc, min_z_mvpLoc;

Skybox skybox;
vector<Omnilight*> lights = vector<Omnilight*>();
Omnilight* movable_light;
vector<TexturedModel*> models = vector<TexturedModel*>();
vector<Model*> mirror_faces = vector<Model*>();
vector<PortalPair*> portals = vector<PortalPair*>();

Camera camera = Camera(0, 0, glm::vec3(), 0, 0);
float init_yaw = 0, init_pitch = 0;
glm::vec3 init_pos = glm::vec3(0, 0, 4);
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
    TexturedModel* model = makeStaticCube(1, { 0, 0, -1.5 }, glm::identity<glm::mat4>(), texture);
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

    Model* mirror;
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
    mirror = new Model(6, vertices);
    //mirror_faces.push_back(mirror);

    float length2 = 2;
    p1 = { -length2, -length2, 1.5 };
    p3 = { -length2, length2, 1.5 };
    p2 = { length2, -length2, 1.5 };
    p4 = { length2, length2, 1.5 };
    vertices = new Vertex[6];
    vertices[0] = { p1, {0, 0} };
    vertices[1] = { p2, {0, 0} };
    vertices[2] = { p4, {0, 0} };
    vertices[3] = { p1, {0, 0} };
    vertices[4] = { p3, {0, 0} };
    vertices[5] = { p4, {0, 0} };
    mirror = new Model(6, vertices);
    //mirror_faces.push_back(mirror);

    glm::vec2 triangles[] = {
        { -0.5, 0.0 }, { 1.5, 4.0 }, { 1.5, 0.0 },
        { -0.5, 0.0 }, { 1.5, 4.0 }, { -0.5, 4.0 },
        { -0.5, 0.0 }, { -0.5, 1.0 }, { -3.5, 1.0 },
        { -0.5, 0.0 }, { -3.5, 0.0 }, { -3.5, 1.0 },
        { -0.5, 4.0 }, { -0.5, 1.0 }, { -3.5, 1.0 },
        { -0.5, 4.0 }, { -1.5, 4.0 }, { -1.5, 3.0 },
        { -2.5, 2.0 }, { -2.5, 4.0 }, { -1.5, 3.0 },
        { 1.5, 0.0 }, { 2.5, 0.0 }, { 1.5, 1.0 },
        { 1.5, 2.0 }, { 2.5, 3.0 }, { 1.5, 4.0 },
        { -1.5, 4.0 }, { -0.5, 5.0 }, { 0.5, 4.0 },
        { 0.5, 5.0 }, { -0.5, 5.0 }, { 0.5, 4.0 },

        { -3.5, 2.0 }, { -3.5, 3.5 }, { -2.75, 2.75 },
        { -3.5, 2.0 }, { -3.5, 3.5 }, { -4.25, 2.75 },
        { 2.5, 1.0 }, { 2.5, 2.0 }, { 3.0, 1.5 },
        { 2.5, 1.0 }, { 2.5, 2.0 }, { 2.0, 1.5 },
        { 1.0, 4.5 }, { 2.0, 4.5 }, { 2.0, 5.5 },
        { 1.0, 4.5 }, { 1.0, 5.5 }, { 2.0, 5.5 },
    };
    
    Plane plane1 = Plane({ 0.0, 0.0, 0.0 }, { 1.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 });
    Portal* portal1 = new Portal(plane1, { 0.0, 0.0, 0.0 }, { 0.2, 0.0, 0.0 }, { 0.0, 0.4, 0.0 }, sizeof(triangles) / sizeof(triangles[0]), triangles);
    Plane plane2 = Plane({ 0.0, 0.0, -1.0 }, { 1.0, 0.0, 0.0 }, { 1.0, 1.0, 0.0 });
    Portal* portal2 = new Portal(plane2, { -10.0, 0.0, 10.0 }, { 0.2, 0.0, 0.2 }, { 0.0, 0.4, 0.0 }, sizeof(triangles) / sizeof(triangles[0]), triangles);
    PortalPair *pair = new PortalPair(portal1, portal2);
    portals.push_back(pair);
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
        float angle = full_time / 20;
        movable_light->light_pos = glm::vec4(2 * glm::sin(angle), 1, 2 * glm::cos(angle), 0);

        // physics
        glm::vec3 prev_pos = camera.getPosition();
        controller->moveCamera(avg);
        glm::vec3 cur_pos = camera.getPosition();

        for (PortalPair* portal : portals)
        {
            if (camera.isTeleported())
                break;
            Portal* portal1 = portal->portal1;
            Portal* portal2 = portal->portal2;
            if (portal1->needTeleport(prev_pos, cur_pos))
                portal->teleportTo2(camera);
            else if (portal2->needTeleport(prev_pos, cur_pos))
                portal->teleportTo1(camera);
        }
    }
    prevFrame = curFrame;
    glutPostRedisplay();
}

void renderRegularObjects()
{
    for (TexturedModel* model : models)
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

void renderPortalFace(Model* model, std::array<glm::vec3, 3> exit_portal_plane, glm::mat4x4 proj, glm::mat4x4 rot, glm::vec3 pos)
{
    glEnable(GL_STENCIL_TEST);

    // set up stencil
    glStencilOp(GL_ZERO, GL_ZERO, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 1);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDepthMask(GL_FALSE);
    model->draw();
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilFunc(GL_EQUAL, 1, 1);

    /*glUseProgram(program_write_min_z);
    glUniformMatrix4fv(min_z_mvpLoc, 1, GL_FALSE, camera.getMvpLoc());
    glBindFramebuffer(GL_FRAMEBUFFER, min_z_fbo);
    glDepthFunc(GL_GREATER);
    glDisable(GL_ALPHA_TEST);
    model->draw();
    glEnable(GL_ALPHA_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

    clearDepthRespectsStencil(1.0);
    /*min_zLoc = glGetUniformLocation(program, "min_z");
    glUniform1i(min_zLoc, min_z_texture);
    glActiveTexture(GL_TEXTURE0 + min_z_texture);
    glBindTexture(GL_TEXTURE_2D, min_z_texture);*/

    // render mirrored objects
    glm::mat4x4 mvp_centered = proj * rot;
    glm::mat4x4 mv = rot * glm::translate(-pos);
    Plane plane2 = Plane(mv * glm::vec4(exit_portal_plane[0], 1.0),
        mv * glm::vec4(exit_portal_plane[1], 1.0),
        mv * glm::vec4(exit_portal_plane[2], 1.0));
    glm::mat4x4 mvp = plane2.clipNearPlane(proj) * mv;

    glUniform3fv(cameraLoc, 1, &pos[0]);
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);
    renderRegularObjects();

    /*clearMinZ();*/

    // render mirrored skybox
    skybox.draw(pos, &mvp_centered[0][0]);

    glUseProgram(program);
    glUniform3fv(cameraLoc, 1, camera.getPosLoc());
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, camera.getMvpLoc());
    // set correct z-buffer
    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 0, 1);
    glDepthFunc(GL_ALWAYS);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    model->draw();
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthFunc(GL_LEQUAL);

    glDisable(GL_STENCIL_TEST);
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
        Plane plane = Plane(mirror->vertices[0].position, mirror->vertices[1].position, mirror->vertices[2].position);
        glm::vec3 pos_flipped = plane.flip(camera.getPosition());
        glm::mat4x4 rot_flipped = plane.flipRotation(camera.getRot());
        std::array<glm::vec3, 3> mirror_plane = { mirror->vertices[0].position, mirror->vertices[1].position, mirror->vertices[2].position };
        renderPortalFace(mirror, mirror_plane, camera.getProj(), rot_flipped, pos_flipped);
    }

    glm::mat4x4 camLocalToWorld = glm::translate(camera.getPosition()) * glm::transpose(camera.getRot());
    for (PortalPair* portal : portals)
    {
        Portal *portal1 = portal->portal1;
        Portal *portal2 = portal->portal2;
        // build inversed transformation in order to easy get rot and pos
        glm::mat4x4 mv1 = portal2->getLocalToWorld() * portal1->getWorldToLocal() * camLocalToWorld;
        glm::vec3 pos1 = glm::vec3(mv1[3]);
        glm::mat4x4 rot1 = mv1;
        rot1[3][0] = rot1[3][1] = rot1[3][2] = 0;
        renderPortalFace(portal1->model, portal2->getPoints(), camera.getProj(), glm::transpose(rot1), pos1);
        glm::mat4x4 mv2 = portal1->getLocalToWorld() * portal2->getWorldToLocal() * camLocalToWorld;
        glm::vec3 pos2 = glm::vec3(mv2[3]);
        glm::mat4x4 rot2 = mv2;
        rot2[3][0] = rot2[3][1] = rot2[3][2] = 0;
        renderPortalFace(portal2->model, portal1->getPoints(), camera.getProj(), glm::transpose(rot2), pos2);
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