#version 430 core

uniform sampler2D colorBuffer;
uniform sampler2D motionBuffer;
uniform sampler2D depthBuffer;
uniform float blur_strength = 0.5;

in vec2 tex_coord;
out vec4 color;

void main() {
    gl_FragDepth = texture(depthBuffer, tex_coord).x;
    vec2 motion = texture(motionBuffer, tex_coord).xy * blur_strength;

    vec4 res = vec4(0.0);

    vec2 coord = tex_coord;
    res += texture(colorBuffer, coord) * 0.4;
    coord -= motion;
    res += texture(colorBuffer, coord) * 0.3;
    coord -= motion;
    res += texture(colorBuffer, coord) * 0.2;
    coord -= motion;
    res += texture(colorBuffer, coord) * 0.1;

    color = res;
    color = texture(colorBuffer, tex_coord);
}