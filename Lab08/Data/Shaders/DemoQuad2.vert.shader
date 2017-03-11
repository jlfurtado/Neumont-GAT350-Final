#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 offset;

out vec3 fColor;

layout(location = 32) uniform float width;
layout(location = 33) uniform float height;
layout(location = 34) uniform int numRows;
layout(location = 35) uniform int numColumns;


void main()
{
	int x = gl_InstanceID % numRows;
	int y = gl_InstanceID / numRows;
	float xPos = 1.0f - (2.0f * (x + 0.5f) / numRows);
	float yPos = 1.0f - (2.0f * (y + 0.5f) / numColumns);
	gl_Position = vec4(position * vec3(width, height, 1.0f) + vec3(xPos, yPos, 0.0f), 1.0f);
	fColor = color;
}