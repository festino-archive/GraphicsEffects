#version 430 core

uniform samplerCube skyboxMap;

in vec3 fromCamera;
out vec4 color;

void main() {
	color = texture(skyboxMap, -fromCamera);
}