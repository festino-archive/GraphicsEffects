#pragma once

#include <GL/glew.h>
#include "Portal.h"

class PortalPair
{
public:
	Portal *portal1, *portal2;

	PortalPair(Portal *portal1, Portal *portal2)
	{
		this->portal1 = portal1;
		this->portal2 = portal2;
	}
};