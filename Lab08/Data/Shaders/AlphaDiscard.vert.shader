#version 430 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vertUV;
layout(location = 2) in vec3 vertexNormal;

layout(location = 13) uniform mat4 modelToWorld;
layout(location = 16) uniform mat4 worldToView;
layout(location = 17) uniform mat4 projection;
layout(location = 10) uniform vec3 tint;


out vec3 lightColor;
out vec3 fragNormWorld;
out vec3 fragPosWorld;
out vec2 UV;

void main()
{
	lightColor = tint;
	gl_Position = projection * worldToView * modelToWorld * vec4(vPos, 1.0f);
	fragNormWorld = normalize(mat3(transpose(inverse(modelToWorld))) * vertexNormal);
	fragPosWorld = vec3(modelToWorld * vec4(vPos, 1.0f));
	UV = vertUV;
}