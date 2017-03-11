#version 430 core

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

in Vertex
{
	vec3 normalView;
	vec3 color;
} vert[];

out vec3 fragColor;

layout(location = 17) uniform mat4 projection;
layout(location = 29) uniform float explodeDist;

void main()
{
	for (int i = 0; i < gl_in.length(); ++i)
	{
		gl_Position = projection * vec4((normalize(vert[i].normalView) * explodeDist) + vec3(gl_in[i].gl_Position), 1.0f);
		fragColor = vert[i].color;
		EmitVertex();
	}

	EndPrimitive();
}