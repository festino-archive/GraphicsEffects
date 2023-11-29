#version 330 core

uniform sampler2D depth_map;
uniform vec3 camera_pos;
uniform float near;
uniform float far;

uniform vec3 fog_color = vec3(0.55, 0.5, 0.55);
uniform vec3 ball_center = vec3(-50.0, 0.0, 50.0);
uniform float ball_radius2 = 900;
uniform float ball_density = 1.0f / 20.0f;

in vec3 dir_unnorm;
out vec4 color;

void main() {
	vec3 dir = normalize(dir_unnorm);
	float depth = texture(depth_map, gl_FragCoord.xy / vec2(1280, 720)).x;
	float z = -near * far / (depth * (far - near) - far);
	vec3 relative = camera_pos - ball_center;
	float b = dot(relative, dir);
	float D1 = b * b + ball_radius2 - dot(relative, relative);
	float alpha = 0;
	if (D1 > 0) {
		float sD1 = sqrt(D1);
		float t1 = -b + sD1;
		float t2 = -b - sD1;
		if (t1 > t2) {
			float t = t1;
			t1 = t2;
			t2 = t;
		}
		if (t2 > 0 && t1 < z) {
		    if (t1 < 0)
				t1 = 0;
			t2 = min(t2, z);
			alpha = ball_density * (t2 - t1);
		}
	}
	color = vec4(fog_color, alpha);
	//color = vec4(vec3(depth), 1);
	//color = vec4(dir * 0.5f + 0.5f, 1);
}