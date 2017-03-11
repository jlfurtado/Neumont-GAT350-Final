#version 430 core

in vec3 lightColor;
in vec3 fragNormWorld;
in vec3 fragPosWorld;
out vec4 fColor;

struct SpotlightInfo
{
	vec4 positionEye;
	vec3 intensity;
	vec3 direction;
	float attenuation;
	float cutoffAngleRadians;
};

layout(location = 4) uniform vec3 diffuseLightColor;
layout(location = 6) uniform vec3 ambientLightColor;
layout(location = 9) uniform vec3 specularLightColor;
layout(location = 7) uniform float specularPower;
layout(location = 11) uniform vec3 cameraPosition_WorldSpace;
layout(location = 18) uniform SpotlightInfo spotLight;

void GetEyeSpace(out vec3 positionEye, out vec3 normalNormEye)
{
	positionEye = normalize(vec3(spotLight.positionEye) - fragPosWorld);
	normalNormEye = normalize(fragNormWorld);
}

vec3 CalculatePhongSpotlight(vec3 positionEye, vec3 normalNormEye)
{
	vec3 minusSNorm = normalize(fragPosWorld - vec3(spotLight.positionEye));
	float dotResult = dot(minusSNorm, normalize(spotLight.direction));
	float theta = acos(dotResult);
	float spotFactor = pow(dotResult, spotLight.attenuation);
	vec3 ambient = ambientLightColor * spotLight.intensity;

	vec3 lightResult = ambient;

	if (theta < spotLight.cutoffAngleRadians)
	{
		vec3 diffuse = diffuseLightColor * spotLight.intensity * max(dot(normalNormEye, positionEye), 0.0f);
		vec3 viewDirection = normalize(cameraPosition_WorldSpace - fragPosWorld);
		vec3 reflectionDirection = reflect(normalize(fragPosWorld - vec3(spotLight.positionEye)), normalNormEye);
		vec3 specular = specularLightColor * spotLight.intensity * pow(max(dot(viewDirection, reflectionDirection), 0.0f), specularPower);
		lightResult += (spotFactor * (diffuse + specular));
	}

	return clamp(lightResult * lightColor, 0.0f, 1.0f);
}

void main()
{
	vec3 positionEye;
	vec3 normalNormEye;

	GetEyeSpace(positionEye, normalNormEye);
	vec3 light = CalculatePhongSpotlight(positionEye, normalNormEye);
	fColor = vec4(light, 1.0f);
}