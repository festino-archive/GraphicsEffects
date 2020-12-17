#version 430 core

uniform sampler2D colorBuffer;
uniform sampler2D motionBuffer;
uniform sampler2D depthBuffer;
uniform float blur_strength = 0.2;

in vec2 tex_coord;
out vec4 color;

void main() {
    gl_FragDepth = texture(depthBuffer, tex_coord).x;
    vec2 motion = texture(motionBuffer, tex_coord).xy * blur_strength;
    vec4 res = vec4(0.0);

    vec2 coord = tex_coord;
    float impact = 0.4;
    float full_impact = 0;
    while (impact > 0 && 0.0 <= coord.x && coord.x <= 1.0 && 0.0 <= coord.y && coord.y <= 1.0)
    {
        res += texture(colorBuffer, coord) * impact;
        full_impact += impact;
        impact -= 0.1;
        coord -= motion;
    }

    color = res / full_impact;
}