#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

flat out vec3 lightColor;

layout(location = 13) uniform mat4 modelToWorld;
layout(location = 16) uniform mat4 worldToView;
layout(location = 17) uniform mat4 projection;
layout(location = 10) uniform vec3 tint;
layout(location = 15) uniform vec3 lightPos_WorldSpace;
layout(location = 4) uniform vec3 diffuseLightColor;
layout(location = 3) uniform vec3 diffuseLightIntensity;
layout(location = 6) uniform vec3 ambientLightColor;
layout(location = 5) uniform vec3 ambientLightIntensity;
layout(location = 9) uniform vec3 specularLightColor;
layout(location = 8) uniform vec3 specularLightIntensity;
layout(location = 7) uniform float specularPower;
layout(location = 11) uniform vec3 cameraPosition_WorldSpace;

void GetEyeSpace(out vec3 positionEye, out vec3 normalNormEye)
{
	vec3 fragPosWorld = vec3(modelToWorld * vec4(vertexPosition, 1.0f));
	positionEye = normalize(lightPos_WorldSpace - fragPosWorld);
	normalNormEye = normalize(mat3(transpose(inverse(modelToWorld))) * vertexNormal);
}

vec3 CalculatePhongLight(vec3 positionEye, vec3 normalNormEye)
{
	vec3 fragPosWorld = vec3(modelToWorld * vec4(vertexPosition, 1.0f));
	vec3 diffuseLight = diffuseLightColor * diffuseLightIntensity * max(dot(normalNormEye, positionEye), 0.0f);

	vec3 ambientLight = ambientLightColor * ambientLightIntensity;

	vec3 viewDirection = normalize(cameraPosition_WorldSpace - fragPosWorld);
	vec3 reflectionDirection = reflect(normalize(fragPosWorld - lightPos_WorldSpace), normalNormEye);
	vec3 specularLight = specularLightColor * specularLightIntensity * pow(max(dot(viewDirection, reflectionDirection), 0.0f), specularPower);

	vec3 totalLight = ambientLight + diffuseLight + specularLight;
	return clamp((totalLight)* tint, 0.0f, 1.0f);
}

void main()
{
	vec3 positionEye;
	vec3 normalNormEye;

	GetEyeSpace(positionEye, normalNormEye);
	lightColor = CalculatePhongLight(positionEye, normalNormEye);

	// calculate position of vertex
	gl_Position = projection * worldToView * modelToWorld * vec4(vertexPosition, 1.0f);
}
