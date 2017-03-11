#version 430 core

in vec4 color;
out vec4 fColor;

layout(location = 10) uniform vec3 tintColor;
layout(location = 11) uniform float tintIntensity;

void main()
{
	fColor = color;
	fColor = (1.0f - tintIntensity) * fColor + tintIntensity * vec4(tintColor, 1.0f);
}