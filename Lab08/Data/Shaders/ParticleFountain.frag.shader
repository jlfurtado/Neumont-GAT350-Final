#version 430 core

in vec2 UV;
in float transparency;
out vec4 outColor;

layout(location = 18) uniform sampler2D textureSampler;

void main()
{
	outColor = texture(textureSampler, UV);
	outColor.a *= transparency;
}
