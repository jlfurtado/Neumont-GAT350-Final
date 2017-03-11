#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

out vec3 lightColor;

layout(location = 13) uniform mat4 modelToWorld;
layout(location = 16) uniform mat4 worldToView;
layout(location = 17) uniform mat4 projection;
layout(location = 10) uniform vec3 tint;
layout(location = 15) uniform vec3 lightPos_WorldSpace;
layout(location = 4) uniform vec3 diffuseLightColor;
layout(location = 3) uniform vec3 diffuseLightIntensity;

void GetEyeSpace(out vec3 positionEye, out vec3 normalNormEye)
{
	vec3 fragmentPosition_WorldSpace = vec3(modelToWorld * vec4(vertexPosition, 1.0f));
	vec3 fragmentNormal_WorldSpace = mat3(transpose(inverse(modelToWorld))) * vertexNormal;

	positionEye = normalize(lightPos_WorldSpace - fragmentPosition_WorldSpace);
	normalNormEye = normalize(fragmentNormal_WorldSpace);
}

vec3 CalculateDiffuseLight(vec3 positionEye, vec3 normalNormEye)
{
	vec3 diffuseLight = diffuseLightColor * diffuseLightIntensity * max(dot(normalNormEye, positionEye), 0.0f);
	diffuseLight = clamp((diffuseLight) * tint, 0.0f, 1.0f);
	return diffuseLight;
}

void main()
{
	vec3 positionEye;
	vec3 normalNormEye;

	// Get the position and normal in eye space
	GetEyeSpace(positionEye, normalNormEye);

	// Evaluate the lighting equation
	lightColor = CalculateDiffuseLight(positionEye, normalNormEye);

	// calculate position of vertex
	gl_Position = projection * worldToView * modelToWorld * vec4(vertexPosition, 1.0f);
}
