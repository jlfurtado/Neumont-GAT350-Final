#version 430 core

layout(points) in;
layout(triangle_strip, max_vertices=4) out;

in vec4 geomColor[];
in vec3 vPos[];
in vec3 fpw[];
out vec4 fragColor;
out vec2 UV;
out vec2 offset;
out vec3 fragPosWorld;

layout(location = 17) uniform mat4 projection;
layout(location = 21) uniform float halfWidth;
layout(location = 10) uniform vec3 tint;
out vec3 lightColor;

void main()
{
	fragColor = geomColor[0];
	gl_Position = projection * (gl_in[0].gl_Position + vec4(halfWidth, -halfWidth, 0.0f, 0.0f));
	offset = vec2(vPos[0]);
	fragPosWorld = fpw[0];
	UV = vec2(1.0f, 0.0f);
	lightColor = tint;
	EmitVertex();

	fragColor = geomColor[0];
	gl_Position = projection * (gl_in[0].gl_Position + vec4(-halfWidth, -halfWidth, 0.0f, 0.0f));
	offset = vec2(vPos[0]);
	fragPosWorld = fpw[0];
	UV = vec2(0.0f, 0.0f);
	lightColor = tint;
	EmitVertex();
	
	fragColor = geomColor[0];
	gl_Position = projection * (gl_in[0].gl_Position + vec4(halfWidth, halfWidth, 0.0f, 0.0f));
	offset = vec2(vPos[0]);
	fragPosWorld = fpw[0];
	UV = vec2(1.0f, 1.0f);
	lightColor = tint;
	EmitVertex();

	fragColor = geomColor[0];
	gl_Position = projection * (gl_in[0].gl_Position + vec4(-halfWidth, halfWidth, 0.0f, 0.0f));
	offset = vec2(vPos[0]);
	fragPosWorld = fpw[0];
	UV = vec2(0.0f, 1.0f);
	lightColor = tint;
	EmitVertex();

	EndPrimitive();
}