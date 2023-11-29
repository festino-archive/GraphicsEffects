#version 140

uniform float z;
uniform vec2 center_pos;
uniform vec2 half_sizes = vec2(1, 1);
uniform vec2 offset = vec2(0, 0);
uniform vec2 scale = vec2(1, 1);

in vec3 position;
in vec2 texCoords;
out vec2 textureCoords;

void main(void){
	vec2 res = scale *  half_sizes * (position.xy + offset);
	gl_Position = vec4(center_pos + res, z, 1.0);
	textureCoords = texCoords;
}