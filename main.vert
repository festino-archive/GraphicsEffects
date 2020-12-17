#version 430 core

uniform vec3 camera;
uniform float near = 0.1f;
uniform float far = 50.0f;
uniform mat4 mvp;

uniform mat4 modelToWorld;
layout(location = 0) in vec3 modelPos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

out vec3 vertex;
out vec3 toCamera_unnorm;
out vec2 texCoords;
out mat3 TBN;

out vec4 proj_cur;
out vec4 proj_prev;

void main() {
	vec4 worldPos = modelToWorld * vec4(modelPos, 1);
	vec4 proj = mvp * worldPos;

	gl_Position = proj;

	vertex = vec3(worldPos);
	toCamera_unnorm = camera - modelPos;
	texCoords = tex;
	
	vec3 T = normalize(vec3(modelToWorld * vec4(tangent, 0.0)));
	vec3 B = normalize(vec3(modelToWorld * vec4(bitangent, 0.0)));
	vec3 N = normalize(vec3(modelToWorld * vec4(normal, 0.0)));
	TBN = mat3(T, B, N);

	proj_cur = proj;
	proj_prev = proj;
}