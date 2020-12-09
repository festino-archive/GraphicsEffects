#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
//#include <glm/ext/matrix_transform.hpp>
//#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>
#include "Utils.h"

class Camera
{
private:
	float yaw;
	float pitch;
	glm::mat4x4 proj;
	glm::mat4x4 rot;
	glm::vec3 pos;
	glm::mat4x4 mvp_centered;
	glm::mat4x4 mvp;
	float hor_sin;
	float hor_cos;
	float win_width, win_height;
	float fov = 45.0f, near_dist = 0.1f, far_dist = 50.0f;

	void updateProj()
	{
		proj = glm::perspectiveRH(fov, win_width / win_height, near_dist, far_dist);
	}
public:
	glm::mat4x4 getMvp_Centered()
	{
		return mvp_centered;
	}
	float* getMvp_CenteredLoc()
	{
		return &mvp_centered[0][0];
	}
	float* getMvpLoc()
	{
		return &mvp[0][0];
	}
	glm::vec3 getPosition()
	{
		return pos;
	}

	void updateMvp()
	{
		mvp_centered = proj * rot;
		mvp = mvp_centered * glm::translate(-pos);
	}

	glm::mat4x4 flippedMvp_centered(glm::vec3 plane1, glm::vec3 plane2)
	{
		return proj * flipRotation(rot, plane1, plane2);
	}

	void teleport(glm::vec3 to)
	{
		pos = to;
	}

	glm::vec3 getRelative(float cameraX, float cameraY, float cameraZ)
	{
		glm::vec3 forward = rot * glm::vec4(cameraX, 0, 0, 1); // check xyz, check if need inverted rot
		glm::vec3 right = rot * glm::vec4(0, cameraY, 0, 1);
		glm::vec3 up = rot * glm::vec4(0, 0, cameraZ, 1);
		return pos + forward + right + up;
	}

	void move(float cameraX, float cameraY, float cameraZ)
	{
		teleport(getRelative(cameraX, cameraY, cameraZ));
	}

	void moveHor(float forward, float right, float up)
	{
		teleport(pos + glm::vec3(hor_cos * right - hor_sin * forward, up, hor_cos * forward + hor_sin * right));
	}

	void setAngle(float yaw, float pitch)
	{
		float yaw_periods = yaw / glm::two_pi<float>();
		if (yaw_periods < 0 || yaw_periods >= 1)
			yaw -= floor(yaw_periods) * glm::two_pi<float>();
		if (pitch < -glm::half_pi<float>())
			pitch = -glm::half_pi<float>();
		if (pitch > glm::half_pi<float>())
			pitch = glm::half_pi<float>();
		this->yaw = yaw;
		this->pitch = pitch;
		hor_sin = sin(yaw);
		hor_cos = cos(yaw);
		//printf("pos %f, %f, %f\n", pos.x, pos.y, pos.z);
		//printf("angles %f, %f\n", yaw, pitch);
		rot = glm::rotate(pitch, glm::vec3(1, 0, 0));
		rot = rot * glm::rotate(yaw, glm::vec3(0, 1, 0));
		//rot = glm::rotate(yaw, glm::vec3(0, 1, 0));
		//rot = glm::rotate(pitch, glm::vec3(rot * glm::vec4(1, 0, 0, 1))) * rot;
	}

	void addAngle(float dYaw, float dPitch)
	{
		setAngle(yaw + dYaw, pitch + dPitch);
	}

	float getYaw()
	{
		return yaw;
	}

	float getPitch()
	{
		return pitch;
	}

	void updateProjSize(float win_width, float win_height)
	{
		this->win_width = win_width;
		this->win_height = win_height;
		updateProj();
	}

	void setProjParams(float fov, float near_dist, float far_dist) {
		this->fov = fov;
		this->near_dist = near_dist;
		this->far_dist = far_dist;
		updateProj();
	}

	Camera(float yaw, float pitch, glm::vec3 pos, float win_width, float win_height)
	{
		setAngle(yaw, pitch);
		teleport(pos);
		updateProjSize(win_width, win_height);
		updateMvp();
	}
};