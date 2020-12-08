#version 430 core

uniform vec3 camera;
uniform mat4 mvp;
layout(location = 0) in vec3 modelPos;
layout(location = 1) in vec3 normal;
out vec3 vertex;
out vec3 toCamera_unnorm;
out vec3 normal_unnorm;

void main() {
	gl_Position = mvp * vec4(modelPos, 1);
	vertex = modelPos;
	toCamera_unnorm = camera - modelPos;

	//normal_unnorm = normalize(modelPos);
	normal_unnorm = normalize(normal);
}