#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

out vec4 lightColor;

layout(location = 13) uniform mat4 modelToWorld;
layout(location = 16) uniform mat4 worldToView;
layout(location = 17) uniform mat4 projection;
layout(location = 10) uniform vec3 tint;
layout(location = 15) uniform vec3 lightPos_WorldSpace;
layout(location = 4) uniform vec3 diffuseLightColor;
layout(location = 3) uniform vec3 diffuseLightIntensity;

void main()
{
	gl_Position = projection * worldToView * modelToWorld * vec4(vertexPosition, 1.0f);
	vec3 objectColor = tint;

	vec3 fragmentLightPosition_WorldSpace = lightPos_WorldSpace;
	vec3 fragmentPosition_WorldSpace = vec3(modelToWorld * vec4(vertexPosition, 1.0f));
	vec3 fragmentNormal_WorldSpace = mat3(transpose(inverse(modelToWorld))) * vertexNormal;

	vec3 normal = normalize(fragmentNormal_WorldSpace);
	vec3 lightDirection = normalize(fragmentLightPosition_WorldSpace - fragmentPosition_WorldSpace);

	vec3 diffuseLight = diffuseLightColor * diffuseLightIntensity * max(dot(normal, lightDirection), 0.0f);
	lightColor = vec4(clamp((diffuseLight)* objectColor, 0.0f, 1.0f), 1.0f);
}