#pragma once

#include <GL/glut.h>
#include <glm/vec3.hpp>
#include <string>
#include <vector>
#include "Camera.h"
#include "Omnilight.h"

class Controller
{
public:
    bool is_paused = false;
    bool flag_info = true;
    bool lantern = false;
    Omnilight* lantern_obj;
    float sensitivity_hor = 0.0012; // TODO: make pixel independent
    float sensitivity_vert = 0.0015;
    bool holdW = false, holdA = false, holdS = false, holdD = false;
    bool holdSpace = false, holdShift = false;

    float movement_speed = 5.0f;
    glm::vec3 speed;

    int *win_width, *win_height;
    Camera *cam;
    std::vector<Omnilight*>* lights;

    Controller(int *win_width, int *win_height, Camera *cam, std::vector<Omnilight*> *lights)
    {
        this->win_width = win_width;
        this->win_height = win_height;
        this->cam = cam;
        this->lights = lights;

        setPaused(false);
    }

    void switch_lantern()
    {
        if (lantern) {
            std::vector<Omnilight*>::iterator position = std::find(lights->begin(), lights->end(), lantern_obj);
            if (position != lights->end())
                lights->erase(position);
            delete lantern_obj;
            lantern_obj = nullptr;
            lantern = false;
        }
        else {
            lantern_obj = new Omnilight{ {0, 0, 0, 1}, {0.82, 0.4, 0.1, 0}, {0.82, 0.4, 0.1, 0} };
            lights->push_back(lantern_obj);
            lantern = true;
        }
    }

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
        if (key == 'l')
            if (down) {
                switch_lantern();
            }
        if (key == ' ')
            holdSpace = down;
        if (key == 112)
            holdShift = !holdShift;
        //holdShift = glutGetModifiers() & GLUT_ACTIVE_SHIFT;
    }
    void setPaused(bool val)
    {
        is_paused = val;
        if (is_paused) {
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        }
        else {
            glutSetCursor(GLUT_CURSOR_NONE);
        }
    }
    void keyDown(unsigned char key, int x, int y)
    {
        if (key == 27)
            setPaused(!is_paused);
        keyState(key, x, y, true);
    }
    void keySpecial(int key, int x, int y)
    {
        if (key == 27)
            setPaused(!is_paused);
        keyState(key, x, y, true);
    }
    void keyUp(unsigned char key, int x, int y)
    {
        keyState(key, x, y, false);
    }

    void mouseMove(int mx, int my) {
        if (!is_paused) {
            int dx = mx - *win_width / 2;
            int dy = my - *win_height / 2;
            glutWarpPointer(*win_width / 2, *win_height / 2);
            cam->addAngle(dx * sensitivity_hor, dy * sensitivity_vert);
        }
    }

    void moveCamera(float time)
    {
        speed = glm::vec3();
        if (holdW)
            speed.z -= 1;
        if (holdS)
            speed.z += 1;
        if (holdA)
            speed.x -= 1;
        if (holdD)
            speed.x += 1;
        if (holdSpace)
            speed.y += 1;
        if (holdShift)
            speed.y -= 1;

        speed *= time * movement_speed;
        cam->moveHor(speed.z, speed.x, speed.y);
    }
};