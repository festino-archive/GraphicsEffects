#version 330 core

uniform mat4 mvp_inversed;

layout(location = 0) in vec2 screen_pos;
out vec3 dir_unnorm;

void main() {
	gl_Position = vec4(screen_pos, 0.0, 1.0);
	vec4 v1 =  mvp_inversed * vec4(screen_pos, 1.0, 1.0);
	vec4 v2 =  mvp_inversed * vec4(screen_pos, 0.0, 1.0);
	vec4 dir4 = v1 / v1.w - v2 / v2.w;
	dir_unnorm = vec3(dir4);
}