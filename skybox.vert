#version 430 core

uniform vec3 camera;
uniform mat4 mvp_centered;
in vec3 worldPos;
out vec3 fromCamera;

void main() {
	fromCamera = worldPos;
	fromCamera.z = -fromCamera.z; // left-hand system
	vec4 farthest_pos = mvp_centered * vec4(worldPos, 1.0);
	gl_Position = farthest_pos.xyww;
}