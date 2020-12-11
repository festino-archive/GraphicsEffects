#version 430 core

uniform mat4 mvp;
layout(location = 0) in vec3 worldPos;
out float z;

void main() {
	vec4 proj = mvp * vec4(worldPos, 1);
	gl_Position = proj;
	z = -proj.z;
}