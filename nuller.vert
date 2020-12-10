#version 430 core

in vec2 worldPos;
out vec3 screenPos;

void main() {
	gl_Position = vec4(worldPos, 1.0, 1.0);
}