#version 430 core

uniform float z;
in vec2 worldPos;

void main() {
	gl_Position = vec4(worldPos, z, 1.0);
}