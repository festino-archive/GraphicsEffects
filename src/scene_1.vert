#version 330 core

uniform mat4 mvp;
in vec3 modelPos;
out vec3 cubeVertex;

void main() {
	cubeVertex = modelPos;
	gl_Position = mvp * vec4(modelPos, 1.0);
}