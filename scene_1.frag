#version 330 core

uniform float AmbientImpact = 0.1;
uniform vec3 AmbientColor = vec3(0.1, 0.4, 0.8);

uniform float LightImpact = 0.7;
uniform vec3 LightColor = vec3(0.9,1,0.9);
uniform vec3 LightPosition = vec3(1,0.5,1);

uniform float SpecularImpact = 0.2;
uniform vec3 SpecularColor = vec3(0.9,1,0.9);
uniform float shininess = 10;

uniform vec3 objectColor = vec3(1,1,1);
in vec3 vertex;
in vec3 normal_unnorm;
in vec3 toCamera_unnorm;
out vec4 color;

void main() {
	//vec3 normal = from_map;
	vec3 normal = normalize(normal_unnorm);
	vec3 toLight = LightPosition - vertex;
	toLight = normalize(toLight);
	vec3 reflected = 2 * dot(toLight, normal) * normal - toLight;
	reflected = normalize(reflected);
	vec3 toCamera = normalize(toCamera_unnorm);
	
	vec3 ambient = AmbientImpact * AmbientColor;
	vec3 diffuse = dot(toLight, normal) * LightImpact * LightColor;
	diffuse = clamp(diffuse, 0, 1);
	vec3 specular = pow(clamp(dot(toCamera, reflected), 0, 1), shininess) * SpecularImpact * SpecularColor;

	if (dot(toLight, normal) <= 0.0)
	{
		specular = vec3(0.0, 0.0, 0.0);
	}
	vec3 rgb = objectColor * (ambient + diffuse + specular);
	color = vec4(rgb, 0);
	//color = vec4(vertex + vec3(0.5,0.5,0.5), 0);
}