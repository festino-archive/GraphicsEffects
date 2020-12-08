#version 430 core

uniform vec3 camera;
uniform mat4 mvp;
layout(location = 0) in vec3 modelPos;
out vec3 toCamera;

void main() {
	gl_Position = mvp * vec4(modelPos, 1);
	toCamera = camera - modelPos;
}