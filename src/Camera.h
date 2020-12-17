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
	float roll;
	bool is_teleported;
	glm::vec3 pos;
	glm::mat4x4 rot;
	glm::mat4x4 proj;
	glm::mat4x4 translation;
	glm::mat4x4 mvp_centered;
	glm::mat4x4 mvp;
	float hor_sin;
	float hor_cos;
	float win_width, win_height;
	float fov = 45.0f, near_dist = 0.1f, far_dist = 50.0f;

	glm::mat4x4 mvp_centered_prev;
	glm::mat4x4 mvp_prev;

	void updateProj()
	{
		proj = glm::perspectiveRH(fov, win_width / win_height, near_dist, far_dist);
	}
	void setPos(glm::vec3 to)
	{
		pos = to;
		translation = glm::translate(-pos);
	}
public:
	float getNear()
	{
		return near_dist;
	}
	float getFar()
	{
		return far_dist;
	}
	float getFov()
	{
		return fov;
	}
	float getAspect()
	{
		return win_width / win_height;
	}
	glm::mat4x4 getProj()
	{
		return proj;
	}
	glm::mat4x4 getRot()
	{
		return rot;
	}
	glm::mat4x4 getTranslation()
	{
		return translation;
	}
	glm::mat4x4 getMvp()
	{
		return mvp;
	}
	glm::mat4x4 getMvp_centered()
	{
		return mvp_centered;
	}
	glm::vec3 getPosition()
	{
		return pos;
	}

	void updateMvp()
	{
		mvp_centered_prev = mvp_centered;
		mvp_prev = mvp;

		mvp_centered = proj * rot;
		mvp = mvp_centered * translation;
	}

	float* getMvp_centered_prevLoc()
	{
		return &mvp_centered_prev[0][0];
	}
	float* getMvp_prevLoc()
	{
		return &mvp_prev[0][0];
	}

	float* getMvp_centeredLoc()
	{
		return &mvp_centered[0][0];
	}
	float* getMvpLoc()
	{
		return &mvp[0][0];
	}
	float* getPosLoc()
	{
		return &pos[0];
	}

	bool isTeleported()
	{
		return is_teleported;
	}

	void teleport(glm::vec3 to)
	{
		is_teleported = true;
		setPos(to);
	}

	void move(glm::vec3 to)
	{
		is_teleported = false;
		setPos(to);
	}

	glm::vec3 getRelative(float cameraX, float cameraY, float cameraZ)
	{
		glm::mat4x4 antirot = glm::transpose(rot);
		glm::vec3 forward = antirot * glm::vec4(cameraX, 0, 0, 1);
		glm::vec3 right = antirot * glm::vec4(0, cameraY, 0, 1);
		glm::vec3 up = antirot * glm::vec4(0, 0, cameraZ, 1);
		return pos + forward + right + up;
	}

	void move(float cameraX, float cameraY, float cameraZ)
	{
		move(getRelative(cameraX, cameraY, cameraZ));
	}

	void moveHor(float forward, float right, float up)
	{
		move(pos + glm::vec3(hor_cos * right - hor_sin * forward, up, hor_cos * forward + hor_sin * right));
	}

	void setAngle(glm::mat4x4 rot)
	{
		float new_yaw = atan2(rot[0][1], rot[0][0]);
		float new_pitch = atan2(-rot[0][2], sqrt(rot[1][2] * rot[1][2] + rot[2][2] * rot[2][2]));
		float new_roll = atan2(rot[1][2], rot[2][2]);
		/* z-y-x
		float new_yaw = atan2(rot[0][1], rot[0][0]);
		float new_pitch = atan2(-rot[0][2], sqrt(rot[1][2] * rot[1][2] + rot[2][2] * rot[2][2]));
		float new_roll = atan2(rot[1][2], rot[2][2]);*/
		/* z-x-z
		float new_yaw = atan2(rot[0][2], rot[1][2]);
		float new_pitch = acos(rot[2][2]);
		float new_roll = -atan2(rot[2][0], rot[2][1]);*/
		setAngle(new_yaw, new_pitch);
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
		this->roll = 0;
		hor_sin = sin(yaw);
		hor_cos = cos(yaw);
		rot = glm::rotate(pitch, glm::vec3(1, 0, 0));
		rot = rot * glm::rotate(roll, glm::vec3(0, 0, 1));
		rot = rot * glm::rotate(yaw, glm::vec3(0, 1, 0));
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