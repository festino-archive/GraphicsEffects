#version 430 core

uniform vec3 camera;
uniform float near = 0.1f;
uniform float far = 50.0f;
uniform mat4 mvp;
uniform sampler2D min_z;

layout(location = 0) in vec3 modelPos;
layout(location = 1) in vec2 tex;

out vec3 vertex;
out vec3 toCamera_unnorm;
out vec2 texCoords;

void main() {
	vec4 proj = mvp * vec4(modelPos, 1);
	//float z_shift = -0.1f + (2.0 * 0.1f * 50.0f) / (50.0f + 0.1f - (texture(min_z, (proj.xy + 1.0) / 2).z * 2.0 - 1.0) * (50.0f - 0.1f));
	//proj.z = proj.z - z_shift;
	//float z_min = (2.0 * near ) / (far + near - (texture(min_z, proj.xy).z * 2.0 - 1.0) * (far - near));
	//float z = proj.z / proj.w;
	//z = (2.0 * near ) / (far + near - (z * 2.0 - 1.0) * (far - near));
	//proj.z = z + (near - z_min) * proj.w;

	//float z_min = -0.1f + (2.0 * texture(min_z, proj.xy).z - (far + near)) / (far - near);
	//proj.z = proj.z - z_min;

	float z_min = (2.0 * near * far) / (far + near - (texture(min_z, proj.xy).z * 2.0 - 1.0) * (far - near));
	proj.z = proj.z - z_min;

	gl_Position = proj;

	vertex = modelPos;
	toCamera_unnorm = camera - modelPos;
	texCoords = tex;
}