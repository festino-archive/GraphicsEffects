#version 430 core

uniform sampler2D skyboxMap;

in vec3 toCamera_unnorm;
out vec4 color;

void main() {
	color = textureCube(skyboxMap, toCamera_unnorm);
}