#version 430 core

in vec2 pos;
out vec2 tex_coord;

void main() {
	gl_Position = vec4(pos, 0, 1.0);
	tex_coord = (pos + 1.0) * 0.5;
}