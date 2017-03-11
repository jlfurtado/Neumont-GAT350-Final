#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in mat4 instanceMatrix;

out vec2 TexCoords;

layout(location = 16) uniform mat4 worldToView;
layout(location = 17) uniform mat4 projection;
layout(location = 13) uniform mat4 modelToWorld;

void main()
{
	gl_Position = projection * worldToView * modelToWorld * instanceMatrix * vec4(position, 1.0f);
	TexCoords = texCoords;
}