#version 330 core

uniform vec4 MaterialSpecular = vec4(0.9,0.9,0.9,1.0);
uniform vec4 MaterialAmbient = vec4(0.2,0.2,0.2,1.0);
uniform vec4 MaterialDiffuse = vec4(0.7,0.7,0.7,1.0);
uniform float Shininess = 10.0;

uniform vec4 LightDiffuse = vec4(1.0,1.0,1.0,1.0);
uniform vec4 LightAmbient = vec4(0.2,0.2,0.2,1.0);

uniform sampler2D normalMap;

vec4 GetAmbientReflection(
	vec4 surfaceColor,
    vec4 lightColor) 
{
	return lightColor * surfaceColor;
}

vec4 GetDiffuseReflection(
	vec4 surfaceColor,
	vec3 surfaceNormal,
    vec4 lightColor,
	vec3 lightDirection) 
{
	float diffuseFactor = 
		clamp(
			dot(lightDirection, surfaceNormal),0.0,1.0);
	return lightColor * surfaceColor * diffuseFactor;
}

vec4 GetSpecularReflection(
	vec4 surfaceColor,
    float  surfaceShininess,
    vec3 surfaceNormal,
    vec4 lightColor,
    vec3 halfAngle) 
{
	float specularFactor = 
	   pow(clamp(dot(halfAngle, surfaceNormal),0.0,1.0), surfaceShininess);
  
	return lightColor * surfaceColor * specularFactor;       
}

vec4 GetBlinnReflection(
	vec4 ambientSurfaceColor,
    vec4 ambientLightColor,
    vec4 diffuseSurfaceColor,
    vec4 specularSurfaceColor,
    float  surfaceShininess,
    vec3 surfaceNormal,
    vec3 halfAngle,
    vec3 lightDirection,
    vec4 lightColor) {
                       
	vec4 ambient = GetAmbientReflection(ambientSurfaceColor, ambientLightColor);

	vec4 diffuse = GetDiffuseReflection(
		diffuseSurfaceColor, 
		surfaceNormal, 
		lightColor, 
		lightDirection);
                                     
	vec4 specular = GetSpecularReflection(
		specularSurfaceColor, 
		surfaceShininess, 
		surfaceNormal,
		lightColor, 
		halfAngle);

	if (dot(lightDirection, surfaceNormal) <= 0.0)
	{
		specular = vec4(0.0, 0.0, 0.0, 0.0);
	}
    
	return diffuse + specular + ambient;
}


in vec3 eyeSurfaceNormal;
in vec3 eyeLightDirection;
in vec3 eyeSurfacePosition;
in vec3 eyeTangent;
in vec3 eyeBinormal;
in vec2 texCoords;

void main()
{	
	vec3 eyeViewerDirection = normalize(-eyeSurfacePosition);
	vec3 nEyeLightDir = normalize(eyeLightDirection);
	vec3 eyeHalfAngle = normalize(normalize(eyeViewerDirection) + nEyeLightDir);

	vec3 bump = (texture2D(normalMap, texCoords.xy).rgb - vec3(0.5, 0.5, 0.5)) * 2.0;

    gl_FragColor = GetBlinnReflection(
		MaterialDiffuse, 
		LightAmbient,
		MaterialDiffuse, 
		MaterialSpecular, 
		Shininess,
		normalize(bump.r * eyeSurfaceNormal + bump.b * eyeBinormal + bump.g * eyeTangent), 
		eyeHalfAngle,
		nEyeLightDir, 
		LightDiffuse);
}
