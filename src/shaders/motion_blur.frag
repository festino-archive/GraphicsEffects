#version 430 core

uniform sampler2D colorBuffer;
uniform sampler2D motionBuffer;
uniform sampler2D depthBuffer;
uniform float blur_strength = 0.3;
uniform float step = 1.0f / 2000;
uniform float impact_factor = 0.998;

in vec2 tex_coord;
out vec4 color;

void main() {
    gl_FragDepth = texture(depthBuffer, tex_coord).x;

    vec2 motion = texture(motionBuffer, tex_coord).xy * blur_strength;
    float factor = pow(impact_factor, 1 / length(motion));
    float impact = 1;
    float full_impact = 0;
    
    motion = normalize(motion) * step;
    vec2 coord = tex_coord;

    vec4 res = texture(colorBuffer, coord);
    coord -= motion;
    full_impact += impact;
    while (impact > step && 0.0 <= coord.x && coord.x <= 1.0 && 0.0 <= coord.y && coord.y <= 1.0)
    {
        impact *= factor;
        res += texture(colorBuffer, coord) * impact;
        coord -= motion;
        full_impact += impact;
    }

    color = res / full_impact;
}