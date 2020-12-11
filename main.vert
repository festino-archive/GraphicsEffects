#version 430 core

uniform sampler2D min_z;

uniform vec3 camera;
uniform mat4 mvp;
layout(location = 0) in vec3 modelPos;
layout(location = 1) in vec2 tex;
out vec3 vertex;
out vec3 toCamera_unnorm;
out vec2 texCoords;

void main() {
	vec4 proj = mvp * vec4(modelPos, 1);
	float z = (2.0 * 0.1f * 50.0f) / (50.0f + 0.1f - (texture(min_z, proj.xy).z * 2.0 - 1.0) * (50.0f - 0.1f));
	//proj.z = proj.z + 0.1 - z;
	gl_Position = proj;

	vertex = modelPos;
	toCamera_unnorm = camera - modelPos;
	texCoords = tex;
}