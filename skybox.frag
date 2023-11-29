#version 430 core

uniform samplerCube skyboxMap;

in vec3 fromCamera;
in vec4 proj_cur;
in vec4 proj_prev;

layout(location = 0) out vec4 color;
layout(location = 1) out vec2 motion;

void main() {
	color = texture(skyboxMap, -fromCamera);

	vec3 ndc_cur = (proj_cur / proj_cur.w).xyz;
    vec3 ndc_prev = (proj_prev / proj_prev.w).xyz;
    motion = (ndc_cur - ndc_prev).xy;
}