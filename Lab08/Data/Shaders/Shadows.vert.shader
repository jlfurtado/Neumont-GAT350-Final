#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

out ShadowBlock
{
	vec3 lightColor;
	vec3 fragNormWorld;
	vec3 fragPosWorld;
	vec4 shadowCoord;
} outData;

layout(location = 13) uniform mat4 modelToWorld;
layout(location = 16) uniform mat4 worldToView;
layout(location = 17) uniform mat4 projection;
layout(location = 10) uniform vec3 tint;
layout(location = 27) uniform mat4 shadowMatrix;

void main()
{
	outData.lightColor = tint;
	outData.fragNormWorld = normalize(mat3(transpose(inverse(modelToWorld))) * vertexNormal);
	outData.fragPosWorld = vec3(modelToWorld * vec4(vertexPosition, 1.0f));
	outData.shadowCoord = shadowMatrix * modelToWorld * vec4(vertexPosition, 1.0f);

	// calculate position of vertex
	gl_Position = projection * worldToView * modelToWorld * vec4(vertexPosition, 1.0f);
}
