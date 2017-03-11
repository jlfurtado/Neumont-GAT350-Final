#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 13) uniform mat4 modelToWorld;
layout(location = 16) uniform mat4 worldToView;
layout(location = 10) uniform vec3 tint;

out Vertex
{
	vec3 normalView;
	vec3 color;
} vert;

void main()
{
	vert.normalView = normalize(mat3(inverse(transpose(worldToView * modelToWorld))) * vertexNormal);
	vert.color = tint;

	// calculate eye space for vertex
	gl_Position = worldToView * modelToWorld * vec4(vertexPosition, 1.0f);
}
