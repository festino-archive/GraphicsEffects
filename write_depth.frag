#version 430 core

in float z;
layout(location = 0) out float depth;

void main() {
	depth = gl_FragCoord.z;
	//depth = z;
}