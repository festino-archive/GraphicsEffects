#version 430 core

uniform float AmbientImpact = 0.1;
uniform vec4 AmbientColor = vec4(0.1, 0.4, 0.8, 0);

struct Omnilight
{
    vec4 position;
    vec4 lightColor;
    vec4 specColor;
	float lightImpact;
	float specImpact;
};
uniform uint lightsCount;
layout (std140, binding = 0) buffer lightsBlock {
	Omnilight lights[];
};

uniform vec4 LightPosition = vec4(1,0.5,1,0);
uniform float LightImpact = 0.7;
uniform vec4 LightColor = vec4(0.9,1,0.9,0);
uniform float SpecularImpact = 0.2;
uniform vec4 SpecularColor = vec4(0.9,1,0.9,0);

uniform float shininess = 10;
uniform vec4 objectColor = vec4(1,1,1,0);
in vec3 vertex;
in vec3 normal_unnorm;
in vec3 toCamera_unnorm;
out vec4 color;

void main() {
	//vec3 normal = from_map;
	vec4 normal = vec4(normalize(normal_unnorm), 0);
	vec4 toCamera = vec4(normalize(toCamera_unnorm), 0);

	vec4 ambient = AmbientImpact * AmbientColor;
	vec4 rgb = ambient;

	for (int i = 0; i < lights.length(); i++) {
		vec4 toLight = lights[i].position - vec4(vertex, 0);
		toLight = normalize(toLight);
		vec4 reflected = 2 * dot(toLight, normal) * normal - toLight;
		reflected = normalize(reflected);
	
		vec4 diffuse = dot(toLight, normal) * lights[i].lightImpact * lights[i].lightColor;
		diffuse = clamp(diffuse, 0, 1);
		vec4 specular = pow(clamp(dot(toCamera, reflected), 0, 1), shininess) * lights[i].specImpact * lights[i].specColor;

		if (dot(toLight, normal) <= 0.0)
		{
			specular = vec4(0,0,0,0);
		}
		rgb += diffuse + specular;
	}

	vec4 toLight = LightPosition - vec4(vertex, 0);
	toLight = normalize(toLight);
	vec4 reflected = 2 * dot(toLight, normal) * normal - toLight;
	reflected = normalize(reflected);
	
	vec4 diffuse = dot(toLight, normal) * LightImpact * LightColor;
	diffuse = clamp(diffuse, 0, 1);
	vec4 specular = pow(clamp(dot(toCamera, reflected), 0, 1), shininess) * SpecularImpact * SpecularColor;

	if (dot(toLight, normal) <= 0.0)
	{
		specular = vec4(0,0,0,0);
	}
	rgb += diffuse + specular;

	rgb *= objectColor;
	color = rgb;
	//color = vec4(vertex + vec3(0.5,0.5,0.5), 0);
}