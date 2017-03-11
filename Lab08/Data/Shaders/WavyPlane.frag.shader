#version 430 core

in vec3 fragNormalEye;
in vec3 lightColor;
in vec3 fragPosEye;
noperspective in vec3 fragEdgeDistance;
out vec4 outColor;

struct LineInfo { float width; vec4 color; };

layout(location = 25) uniform vec2 randomValue;
layout(location = 23) uniform float repeatScale;
layout(location = 24) uniform int numIterations;
layout(location = 15) uniform vec3 lightPos_WorldSpace;
layout(location = 4) uniform vec3 diffuseLightColor;
layout(location = 3) uniform vec3 diffuseLightIntensity;
layout(location = 6) uniform vec3 ambientLightColor;
layout(location = 5) uniform vec3 ambientLightIntensity;
layout(location = 9) uniform vec3 specularLightColor;
layout(location = 8) uniform vec3 specularLightIntensity;
layout(location = 7) uniform float specularPower;
layout(location = 11) uniform vec3 cameraPosition_WorldSpace;
layout(location = 26) uniform LineInfo lineInfo;

void GetEyeSpace(out vec3 positionEye, out vec3 normalNormEye)
{
	positionEye = normalize(lightPos_WorldSpace - fragPosEye);
	normalNormEye = normalize(fragNormalEye);
}

vec3 CalculatePhongLight(vec3 positionEye, vec3 normalNormEye)
{
	vec3 diffuseLight = diffuseLightColor * diffuseLightIntensity * max(dot(normalNormEye, positionEye), 0.0f);

	vec3 ambientLight = ambientLightColor * ambientLightIntensity;

	vec3 viewDirection = normalize(cameraPosition_WorldSpace - fragPosEye);
	vec3 reflectionDirection = reflect(normalize(fragPosEye - lightPos_WorldSpace), normalNormEye);
	vec3 specularLight = specularLightColor * specularLightIntensity * pow(max(dot(viewDirection, reflectionDirection), 0.0f), specularPower);

	vec3 totalLight = ambientLight + diffuseLight + specularLight;
	return clamp((totalLight)* lightColor, 0.0f, 1.0f);
}

void main()
{
	vec3 positionEye;
	vec3 normalNormEye;
	GetEyeSpace(positionEye, normalNormEye);
	if (!gl_FrontFacing) { normalNormEye *= -1.0f; }
	vec3 light = CalculatePhongLight(positionEye, normalNormEye);
	
	if (lineInfo.width > 0.0f)
	{
		float d = min(fragEdgeDistance.x, fragEdgeDistance.y);
		d = min(d, fragEdgeDistance.z);

		float mixVal = smoothstep(lineInfo.width - 1, lineInfo.width + 1, d);
		outColor = mix(lineInfo.color, vec4(light, 1.0f), mixVal);
	}
	else
	{
		outColor = vec4(light, 1.0f);
	}
}
