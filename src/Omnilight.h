#pragma once

#include <glm/vec4.hpp>

#pragma pack(4)

struct Omnilight
{
	glm::vec4 light_pos;
	glm::vec4 light_color;
	glm::vec4 light_spec_color;
};

#pragma pack()