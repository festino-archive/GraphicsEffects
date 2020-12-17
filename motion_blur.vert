#version 430 core

in vec2 pos;
out vec2 tex_coord;

void main() {
    vec2 tex = (pos + 1.0) * 0.5;
	gl_Position = vec4(pos, 0, 1.0);
	tex_coord = tex;
}