#version 430 core

uniform sampler2D guiTexture;
uniform vec4 color_mask;// = vec4(1, 1, 1, 1);

in vec2 textureCoords;
out vec4 out_Color;

void main(void) {
	out_Color = color_mask * texture(guiTexture, textureCoords);
	//out_Color = vec4(textureCoords.x, textureCoords.y, 0, 1);
}