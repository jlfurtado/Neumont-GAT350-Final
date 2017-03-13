#version 430 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec2 vertUV;

out vec2 UV;
out vec3 fPos;

layout(location = 13) uniform mat4 modelToWorld;
layout(location = 16) uniform mat4 worldToView;
layout(location = 17) uniform mat4 projection;

void main()
{
	gl_Position = projection * worldToView * modelToWorld * vec4(vPos, 1.0f);

	UV = vertUV;
	fPos = vec3(modelToWorld * vec4(vPos, 1.0f));
}