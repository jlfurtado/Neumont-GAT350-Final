#version 430 core

in vec4 lightColor;
out vec4 fColor;

void main()
{
	fColor = lightColor;
}