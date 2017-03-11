#version 430 core

in vec3 lightColor;
out vec4 fColor;

void main()
{
	fColor = vec4(lightColor, 1.0f);
}