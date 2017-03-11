#version 430 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vColor;

out vec4 color;

layout(location = 13) uniform mat4 modelToWorld;
layout(location = 16) uniform mat4 worldToView;
layout(location = 17) uniform mat4 projection;
layout(location = 14) uniform vec3 tint;

void main()
{
	gl_Position = projection * worldToView * modelToWorld * vec4(vPos, 1.0f);
	color = vec4(vColor * 0.25f + tint * 0.75f, 1.0f);
}