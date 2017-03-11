#version 430 core

in ShadowBlock
{
	vec3 lightColor;
	vec3 fragNormWorld;
	vec3 fragPosWorld;
} inData;

layout(location = 0) out vec4 ambient;
layout(location = 1) out vec4 diffuseAndSpecular;
layout(location = 33) uniform sampler2D uDiffuseAndSpecularTexture;

layout(location = 15) uniform vec3 lightPos_WorldSpace;
layout(location = 4) uniform vec3 diffuseLightColor;
layout(location = 3) uniform vec3 diffuseLightIntensity;
layout(location = 6) uniform vec3 ambientLightColor;
layout(location = 5) uniform vec3 ambientLightIntensity;
layout(location = 9) uniform vec3 specularLightColor;
layout(location = 8) uniform vec3 specularLightIntensity;
layout(location = 7) uniform float specularPower;
layout(location = 11) uniform vec3 cameraPosition_WorldSpace;
layout(location = 34) uniform float screenToTexWidth;
layout(location = 35) uniform float screenToTexHeight;

subroutine void RenderPassType();
subroutine uniform RenderPassType renderPass;

subroutine(RenderPassType)
void FinalPass()
{
	ivec2 pixelPos = ivec2(vec2((gl_FragCoord.x * screenToTexWidth), (gl_FragCoord.y * screenToTexHeight)));
	vec4 diffSpec = texelFetch(uDiffuseAndSpecularTexture, pixelPos, 0);
	ambient = vec4(diffSpec.xyz, 1.0f); // ambient = !?!?!?!		
}

subroutine(RenderPassType)
void Shade()
{
	vec3 positionEye = normalize(lightPos_WorldSpace - inData.fragPosWorld);
	vec3 normalNormEye = normalize(inData.fragNormWorld);

	vec3 diffuseLight = diffuseLightColor * diffuseLightIntensity * max(dot(normalNormEye, positionEye), 0.0f);

	vec3 ambientLight = ambientLightColor * ambientLightIntensity;

	vec3 viewDirection = normalize(cameraPosition_WorldSpace - inData.fragPosWorld);
	vec3 reflectionDirection = reflect(normalize(inData.fragPosWorld - lightPos_WorldSpace), normalNormEye);
	vec3 specularLight = specularLightColor * specularLightIntensity * pow(max(dot(viewDirection, reflectionDirection), 0.0f), specularPower);

	ambient = vec4(ambientLight, 1.0f);
	diffuseAndSpecular = vec4(diffuseLight + specularLight, 1.0f);
}

subroutine(RenderPassType)
void DoNothing()
{
	// i'm desprate, i'll try anything now - even nothing
}

void main()
{
	renderPass();
}