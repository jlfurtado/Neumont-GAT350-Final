#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 13) uniform mat4 modelToWorld;
layout(location = 16) uniform mat4 worldToView;

out vec3 vPos;
out vec4 geomColor;
out vec3 fpw;

void main()
{
	geomColor = vec4(vertexColor, 1.0f);
	vPos = vertexPosition;
	fpw = vec3(modelToWorld * vec4(vertexPosition, 1.0f));

	// calculate eye space for vertex
	gl_Position = worldToView * modelToWorld * vec4(vertexPosition, 1.0f);
}
