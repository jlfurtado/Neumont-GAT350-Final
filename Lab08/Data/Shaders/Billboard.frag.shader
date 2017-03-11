#version 430 core

in vec2 UV;
in vec4 fragColor;
in vec2 offset;
in vec3 lightColor;
in vec3 fragPosWorld;
out vec4 outColor;

layout(location = 18) uniform sampler2D textureSampler;
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

void GetEyeSpace(out vec3 positionEye, out vec3 normalNormEye)
{
	positionEye = normalize(lightPos_WorldSpace - fragPosWorld);
	normalNormEye = normalize(-fragPosWorld + cameraPosition_WorldSpace);
}

vec3 CalculatePhongLight(vec3 positionEye, vec3 normalNormEye, float f)
{
	vec3 diffuseLight = diffuseLightColor * diffuseLightIntensity * max(dot(normalNormEye, positionEye), 0.0f);

	vec3 ambientLight = ambientLightColor * ambientLightIntensity;

	vec3 viewDirection = normalize(cameraPosition_WorldSpace - fragPosWorld);
	vec3 reflectionDirection = reflect(normalize(fragPosWorld - lightPos_WorldSpace), normalNormEye);
	vec3 specularLight = specularLightColor * specularLightIntensity * pow(max(dot(viewDirection, reflectionDirection), 0.0f), specularPower);

	vec3 totalLight = (texture2D(textureSampler, vec2(f, 0.5f)) * (ambientLight + diffuseLight)) + specularLight;
	return clamp((totalLight)* lightColor, 0.0f, 1.0f);
}

void main()
{
	float repeatScaleOverTwo = repeatScale / 2.0f;

	int xI = int(floor((UV.x + offset.x) / repeatScaleOverTwo));
	int zI = int(floor((UV.y + offset.y) / repeatScaleOverTwo));
	vec2 z = (mod(UV + offset, repeatScale) - vec2(repeatScaleOverTwo, repeatScaleOverTwo));

	if (mod(floor(xI + zI), 2) == 0) { z = z *mat2(-1, 0, 0, 1); } // rotate 180

	int i;
	float f2;
	for (i = 0; i<numIterations; i++) {
		float x = (z.x * z.x - z.y * z.y) + randomValue.x;
		float y = (z.y * z.x + z.x * z.y) + randomValue.y;

		f2 = x * x + y * y;
		if (f2 > 4.0) { break; }

		z.x = x;
		z.y = y;
	}

	float f = (i + f2) / 100.0f;
	if (f < 0.25 || f > 0.75) { discard; }

	vec3 positionEye;
	vec3 normalNormEye;
	GetEyeSpace(positionEye, normalNormEye);
	if (!gl_FrontFacing) { normalNormEye *= -1.0f; }
	vec3 light = CalculatePhongLight(positionEye, normalNormEye, f);
	outColor = vec4(light, 1.0f);
}
