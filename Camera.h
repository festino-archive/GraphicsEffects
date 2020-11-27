#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
//#include <glm/ext/matrix_transform.hpp>
//#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

class Camera
{
private:
	float yaw;
	float pitch;
	glm::vec3 pos;
	glm::mat4x4 proj;
	glm::mat4x4 rot;
	float hor_sin;
	float hor_cos;
public:
	glm::mat4x4 mvp;

	void updateMvp()
	{
		mvp = proj * rot * glm::translate(pos);
	}

	void teleport(glm::vec3 to)
	{
		pos = to;
	}

	void move(float cameraX, float cameraY, float cameraZ)
	{
		glm::vec3 forward = rot * glm::vec4(cameraX, 0, 0, 1); // check xyz, check if need inverted rot
		glm::vec3 right = rot * glm::vec4(0, cameraY, 0, 1);
		glm::vec3 up = rot * glm::vec4(0, 0, cameraZ, 1);
		teleport(pos + forward + right + up);
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

	void updateProj(float win_width, float win_heignt)
	{
		proj = glm::perspectiveRH(45.0f, win_width / win_heignt, 1.0f, 20.0f);
	}

	Camera(float yaw, float pitch, glm::vec3 pos, float win_width, float win_heignt)
	{
		setAngle(yaw, pitch);
		teleport(pos);
		updateProj(win_width, win_heignt);
		updateMvp();
	}
};