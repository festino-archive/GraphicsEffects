#pragma once

#include <GL/glew.h>
#include "Portal.h"
#include "Camera.h"

class PortalPair
{
public:
	Portal *portal1, *portal2;

	PortalPair(Portal *portal1, Portal *portal2)
	{
		this->portal1 = portal1;
		this->portal2 = portal2;
	}

	void teleportTo2(Camera& cam)
	{
		glm::vec3 pos = cam.getPosition();
		glm::mat4x4 mat = portal2->getLocalToWorld() * portal1->getWorldToLocal();
		cam.teleport(mat * glm::vec4(pos, 1.0f));
		//cam.setAngle(mat);
	}

	void teleportTo1(Camera& cam)
	{
		glm::vec3 pos = cam.getPosition();
		glm::mat4x4 mat = portal1->getLocalToWorld() * portal2->getWorldToLocal();
		cam.teleport(mat * glm::vec4(pos, 1.0f));
		//cam.setAngle(mat);
	}
};