#version 430 core

in vec3 lightColor;
in vec3 fragNormWorld;
in vec3 fragPosWorld;
out vec4 fColor;

struct LightInfo
{
	vec4 positionEye;
	vec3 intensity;
};

layout(location = 18) uniform LightInfo lights[3];
layout(location = 4) uniform vec3 diffuseLightColor;
layout(location = 6) uniform vec3 ambientLightColor;
layout(location = 9) uniform vec3 specularLightColor;
layout(location = 7) uniform float specularPower;
layout(location = 11) uniform vec3 cameraPosition_WorldSpace;

void GetEyeSpace(out vec3 positionEye, out vec3 normalNormEye, int index)
{
	positionEye = normalize(vec3(lights[index].positionEye) - fragPosWorld);
	normalNormEye = normalize(fragNormWorld);
}

vec3 CalculatePhongLight(vec3 positionEye, vec3 normalNormEye, int index)
{
	vec3 diffuseLight = diffuseLightColor * lights[index].intensity * max(dot(normalNormEye, positionEye), 0.0f);

	vec3 ambientLight = ambientLightColor * lights[index].intensity;

	vec3 viewDirection = normalize(cameraPosition_WorldSpace - fragPosWorld);
	vec3 reflectionDirection = reflect(normalize(fragPosWorld - vec3(lights[index].positionEye)), normalNormEye);
	vec3 specularLight = specularLightColor * lights[index].intensity * pow(max(dot(viewDirection, reflectionDirection), 0.0f), specularPower);

	vec3 totalLight = ambientLight + diffuseLight + specularLight;
	return (totalLight)* lightColor;
}

void main()
{
	vec3 positionEye;
	vec3 normalNormEye;
	
	vec3 light;
	for (int i = 0; i < 3; ++i)
	{
		GetEyeSpace(positionEye, normalNormEye, i);
		light += CalculatePhongLight(positionEye, normalNormEye, i);
	}
	
	fColor = vec4(clamp(light, 0.0f, 1.0f), 1.0f);
}