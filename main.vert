#version 430 core

uniform vec3 camera;
uniform float near = 0.1f;
uniform float far = 50.0f;
uniform mat4 mvp;

uniform mat4 modelToWorld;
layout(location = 0) in vec3 modelPos;
layout(location = 1) in vec2 tex;

out vec3 vertex;
out vec3 toCamera_unnorm;
out vec2 texCoords;

void main() {
	vec4 proj = mvp * modelToWorld * vec4(modelPos, 1);

	gl_Position = proj;

	vertex = modelPos;
	toCamera_unnorm = camera - modelPos;
	texCoords = tex;
}