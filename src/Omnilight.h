#pragma once

#include <glm/vec3.hpp>

#pragma pack(0)

struct Omnilight
{
	glm::vec3 light_pos;
	float light_impact;
	glm::vec3 light_color;
	float light_spec_impact;
	glm::vec3 light_spec_color;
};

#pragma pack()