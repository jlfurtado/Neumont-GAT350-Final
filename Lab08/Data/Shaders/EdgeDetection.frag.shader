#version 430 core

in vec3 lightColor;
in vec3 fragNormWorld;
in vec3 fragPosWorld;
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
layout(location = 34) uniform float screenToTexWidth;
layout(location = 35) uniform float screenToTexHeight;
layout(location = 33) uniform sampler2D uDiffuseAndSpecularTexture;
layout(location = 18) uniform int numLevels;
layout(location = 40) uniform vec3 edgeColor;

subroutine void RenderPassType();
subroutine uniform RenderPassType renderPass;

layout(location = 36) uniform int radius;

float intensity(in vec4 color)
{
	return sqrt((color.x*color.x) + (color.y*color.y) + (color.z*color.z));
}

float quantize(float value)
{
	return ceil(value * (numLevels - 1)) / (numLevels - 1);
}

vec4 getColorAt(vec2 xy)
{
	ivec2 pixelPos = ivec2(xy.x, xy.y);
	vec4 diffSpec = texelFetch(uDiffuseAndSpecularTexture, pixelPos, 0);
	return diffSpec; 
}

vec3 simple_edge_detection(in float step, in vec2 center)
{
	float center_intensity = intensity(getColorAt(center));
	int darker_count = 0;
	float max_intensity = center_intensity;
	for (int i = -radius; i <= radius; i++)
	{
		for (int j = -radius; j <= radius; j++)
		{
			vec2 current_location = center + vec2(i*step, j*step);
			float current_intensity = intensity(getColorAt(current_location));
			if (current_intensity < center_intensity) { darker_count++; } 
			if (current_intensity > max_intensity)
			{
				max_intensity = current_intensity;
			}
		}
	}

	if ((max_intensity - center_intensity) < 0.01*radius)
	{
		if (darker_count / (radius*radius) < (1 - (1 / radius)))
		{
			return getColorAt(center);
		}
	}
	return edgeColor;
}

subroutine(RenderPassType)
void FinalPass()
{
	float step = 1.0;
	fColor.xyz = simple_edge_detection(step, vec2(gl_FragCoord.x * screenToTexWidth, gl_FragCoord.y * screenToTexHeight));
	fColor.a = 1.0;
}

void GetEyeSpace(out vec3 positionEye, out vec3 normalNormEye)
{
	positionEye = normalize(lightPos_WorldSpace - fragPosWorld);
	normalNormEye = normalize(fragNormWorld);
}

vec3 CalculatePhongLight(vec3 positionEye, vec3 normalNormEye)
{
	vec3 diffuseLight = diffuseLightColor * diffuseLightIntensity * quantize(max(dot(normalNormEye, positionEye), 0.0f));

	vec3 ambientLight = ambientLightColor * ambientLightIntensity;

	vec3 viewDirection = normalize(cameraPosition_WorldSpace - fragPosWorld);
	vec3 reflectionDirection = reflect(normalize(fragPosWorld - lightPos_WorldSpace), normalNormEye);
	vec3 specularLight = specularLightColor * specularLightIntensity * pow(max(dot(viewDirection, reflectionDirection), 0.0f), specularPower);

	vec3 totalLight = ambientLight + diffuseLight;
	return clamp((totalLight)* lightColor, 0.0f, 1.0f);
}

subroutine(RenderPassType)
void Shade()
{
	vec3 positionEye;
	vec3 normalNormEye;

	GetEyeSpace(positionEye, normalNormEye);
	vec3 light = CalculatePhongLight(positionEye, normalNormEye);
	fColor = vec4(light, 1.0f);
}

void main()
{
	renderPass();
}