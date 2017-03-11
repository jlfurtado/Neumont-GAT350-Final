#version 430 core

in ShadowBlock
{
	vec3 lightColor;
	vec3 fragNormWorld;
	vec3 fragPosWorld;
	vec4 shadowCoord;
} inData;

out vec4 fColor;

layout(location = 15) uniform vec3 lightPos_WorldSpace;
layout(location = 4) uniform vec3 diffuseLightColor;
layout(location = 3) uniform vec3 diffuseLightIntensity;
layout(location = 6) uniform vec3 ambientLightColor;
layout(location = 5) uniform vec3 ambientLightIntensity;
layout(location = 9) uniform vec3 specularLightColor;
layout(location = 8) uniform vec3 specularLightIntensity;
layout(location = 7) uniform float specularPower;
layout(location = 11) uniform vec3 cameraPosition_WorldSpace;
layout(location = 28) uniform sampler2DShadow shadowMap;

subroutine void RenderPassType();
subroutine uniform RenderPassType renderPass;

void GetEyeSpace(out vec3 positionEye, out vec3 normalNormEye)
{
	positionEye = normalize(lightPos_WorldSpace - inData.fragPosWorld);
	normalNormEye = normalize(inData.fragNormWorld);
}

vec3 CalculatePhongLight(vec3 positionEye, vec3 normalNormEye, float shadow)
{
	vec3 diffuseLight = diffuseLightColor * diffuseLightIntensity * max(dot(normalNormEye, positionEye), 0.0f);

	vec3 ambientLight = ambientLightColor * ambientLightIntensity;

	vec3 viewDirection = normalize(cameraPosition_WorldSpace - inData.fragPosWorld);
	vec3 reflectionDirection = reflect(normalize(inData.fragPosWorld - lightPos_WorldSpace), normalNormEye);
	vec3 specularLight = specularLightColor * specularLightIntensity * pow(max(dot(viewDirection, reflectionDirection), 0.0f), specularPower);

	vec3 totalLight = ambientLight + (shadow * (diffuseLight + specularLight));
	return clamp((totalLight)* inData.lightColor, 0.0f, 1.0f);
}

bool manageOut()
{
	return inData.shadowCoord.z < 0.0f || inData.shadowCoord.z / inData.shadowCoord.w  > 1.0f;
}

subroutine(RenderPassType)
void PhongWithShadow()
{
	vec3 positionEye;
	vec3 normalNormEye;

	GetEyeSpace(positionEye, normalNormEye);

	float shadow = manageOut() ? 1.0f : textureProj(shadowMap, inData.shadowCoord);
	vec3 light = CalculatePhongLight(positionEye, normalNormEye, shadow);
	fColor = vec4(light, 1.0f);
}

subroutine(RenderPassType)
void PhongWithShadowAverage()
{
	vec3 positionEye;
	vec3 normalNormEye;

	GetEyeSpace(positionEye, normalNormEye);

	float shadow2 = textureProjOffset(shadowMap, inData.shadowCoord, ivec2(1, 1));
	float shadow3 = textureProjOffset(shadowMap, inData.shadowCoord, ivec2(1, -1));
	float shadow4 = textureProjOffset(shadowMap, inData.shadowCoord, ivec2(-1, 1));
	float shadow5 = textureProjOffset(shadowMap, inData.shadowCoord, ivec2(-1, -1));
	float shadow = manageOut() ? 1.0f : 0.25f * (shadow2 + shadow3 + shadow4 + shadow5);

	vec3 light = CalculatePhongLight(positionEye, normalNormEye, shadow);
	fColor = vec4(light, 1.0f);
}

subroutine(RenderPassType)
void RecordDepth()
{
	// does nothing on purpose
}

void main()
{
	renderPass();
}