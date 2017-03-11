#version 430 core

layout(points) in;
layout(triangle_strip, max_vertices=4) out;

in float trans[];
out vec2 UV;
out float transparency;

layout(location = 17) uniform mat4 projection;
layout(location = 21) uniform float halfWidth;

void main()
{
	gl_Position = projection * (gl_in[0].gl_Position + vec4(halfWidth, -halfWidth, 0.0f, 0.0f));
	transparency = trans[0];
	UV = vec2(1.0f, 0.0f);
	EmitVertex();

	gl_Position = projection * (gl_in[0].gl_Position + vec4(-halfWidth, -halfWidth, 0.0f, 0.0f));
	transparency = trans[0];
	UV = vec2(0.0f, 0.0f);
	EmitVertex();
	
	gl_Position = projection * (gl_in[0].gl_Position + vec4(halfWidth, halfWidth, 0.0f, 0.0f));
	transparency = trans[0];
	UV = vec2(1.0f, 1.0f);
	EmitVertex();

	gl_Position = projection * (gl_in[0].gl_Position + vec4(-halfWidth, halfWidth, 0.0f, 0.0f));
	transparency = trans[0];
	UV = vec2(0.0f, 1.0f);
	EmitVertex();

	EndPrimitive();
}