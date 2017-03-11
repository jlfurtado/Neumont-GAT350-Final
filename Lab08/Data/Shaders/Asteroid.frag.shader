#version 430 core

in vec2 TexCoords;

out vec4 color;

layout(location = 18) uniform sampler2D textureSampler;


void main()
{
	color = vec4(texture(textureSampler, TexCoords).xyz, 1.0f);
}