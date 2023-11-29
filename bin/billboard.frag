#version 430 core

uniform sampler2D billboardTexture;
uniform float alpha;

in vec2 textureCoords;
out vec4 out_Color;

void main(void) {
	vec4 color = texture(billboardTexture, textureCoords);
	color.w *= alpha;
	out_Color = color;
}