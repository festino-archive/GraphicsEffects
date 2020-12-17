#version 430 core

uniform float AmbientImpact = 0.1;
uniform vec4 AmbientColor = vec4(0.1, 0.4, 0.8, 0);

struct Omnilight
{
    vec4 position;
    vec4 lightColor;
    vec4 specColor;
};
uniform uint lightsCount;
layout (std140, binding = 0) buffer lightsBlock {
	Omnilight lights[];
};

uniform sampler2D skyboxMap;

uniform float light_impact;
uniform float spec_impact;
uniform float shininess = 1;
uniform sampler2D colorMap;
uniform sampler2D normalMap;

in vec3 vertex;
in vec3 toCamera_unnorm;
in vec2 texCoords;
in mat3 TBN;

in vec4 proj_cur;
in vec4 proj_prev;

layout (location = 0) out vec3 color;
layout (location = 1) out vec2 motion;

void main() {
	vec3 normal_unnorm = vec3(texture2D(normalMap, texCoords));
	normal_unnorm = (normal_unnorm - 0.5f) * 2.0;
	vec3 normal_temp = normalize(normal_unnorm);
	vec4 normal = vec4(normalize(TBN * normal_temp), 0.0);  
	vec4 toCamera = vec4(normalize(toCamera_unnorm), 0.0);

	vec4 ambient = AmbientImpact * AmbientColor;
	vec4 rgb = ambient;

	for (int i = 0; i < lights.length(); i++) {
		vec4 toLight = lights[i].position - vec4(vertex, 0.0);
		toLight = normalize(toLight);
		if (dot(toLight, normal) > 0.0)
		{
			float dist = dot(toLight, toLight);
			vec4 reflected = 2 * dot(toLight, normal) * normal - toLight;
			reflected = normalize(reflected);
	
			vec4 diffuse = light_impact * dot(toLight, normal) * lights[i].lightColor;
			diffuse = clamp(diffuse, 0, 1);
			vec4 specular = spec_impact * pow(clamp(dot(toCamera, reflected), 0, 1), shininess) * lights[i].specColor;
			rgb += (diffuse + specular) / sqrt(dist);
		}
	}

	vec4 objectColor = texture2D(colorMap, texCoords);
	rgb *= objectColor;
	color = vec3(rgb);

	vec3 ndc_cur = (proj_cur / proj_cur.w).xyz;
    vec3 ndc_prev = (proj_prev / proj_prev.w).xyz;
    motion = (ndc_cur - ndc_prev).xy;
}