#version 430 core

uniform vec3 camera;
uniform mat4 mvp_centered;
uniform mat4 mvp_centered_prev;

in vec3 worldPos;

out vec3 fromCamera;
out vec4 proj_cur;
out vec4 proj_prev;

void main() {
	fromCamera = worldPos;
	fromCamera.z = -fromCamera.z; // left-hand system
	vec4 farthest_pos = mvp_centered * vec4(worldPos, 1.0);
	gl_Position = farthest_pos.xyww;

	proj_cur = farthest_pos;
	vec4 farthest_pos_prev = mvp_centered_prev * vec4(worldPos, 1.0);
	proj_prev = farthest_pos_prev;
}