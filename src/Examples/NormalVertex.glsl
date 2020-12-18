#version 330 core

uniform vec4 LightPosition = vec4(0,0,0,1);

in vec3 position;
in vec3 normal;
in vec3 tangent;
in vec3 binormal;
in vec2 tex;

out vec3 eyeSurfaceNormal;
out vec3 eyeLightDirection;
out vec3 eyeSurfacePosition;
out vec3 eyeTangent;
out vec3 eyeBinormal;
out vec2 texCoords;

uniform mat4 mv;
uniform mat4 mvp;
uniform mat3 nm;

void main()
{
	vec4 pos =	mv * vec4(position, 1);	
	eyeSurfacePosition = pos.xyz / pos.w;
	vec3 eyeLightPosition   = LightPosition.xyz / LightPosition.w; 

	eyeSurfaceNormal   = normalize(nm * normal);
	eyeLightDirection  = normalize(eyeLightPosition - eyeSurfacePosition);
	
	eyeBinormal = normalize(nm * binormal);
	eyeTangent = normalize(nm * tangent);
	
	gl_Position = mvp * pos;
	texCoords = vec2(5.0 * tex.x, tex.y);

	gl_Position = mvp * vec4(position, 1);
}
