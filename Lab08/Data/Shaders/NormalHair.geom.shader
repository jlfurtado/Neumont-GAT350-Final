#version 430 core

layout(triangles) in;
layout(line_strip, max_vertices=12) out;

in Vertex
{
	vec3 normalView;
	vec3 color;
} vert[];

out vec3 fragColor;

layout(location = 17) uniform mat4 projection;
layout(location = 27) uniform vec3 vertexNormalColor;
layout(location = 28) uniform vec3 faceNormalColor;
layout(location = 29) uniform float hairLength;
layout(location = 30) uniform int hairState;

void outTriangleAsLines()
{
	for (int i = 0; i < gl_in.length(); ++i)
	{
		gl_Position = projection * gl_in[i].gl_Position;
		fragColor = vert[i].color;
		EmitVertex();
	}

	gl_Position = projection * gl_in[0].gl_Position;
	fragColor = vert[0].color;
	EmitVertex();

	EndPrimitive();
}

void outNormalLineHairs()
{
	for (int i = 0; i < gl_in.length(); ++i)
	{
		gl_Position = projection * (gl_in[i].gl_Position);
		fragColor = vertexNormalColor;
		EmitVertex();

		gl_Position = projection * vec4(vec3(gl_in[i].gl_Position) + normalize(vert[i].normalView) * hairLength, 1.0f);
		fragColor = vertexNormalColor;
		EmitVertex();

		EndPrimitive();
	}
}

void outFaceNormalLineHairs()
{
	vec3 center;
	for (int i = 0; i < gl_in.length(); ++i)
	{
		center += vec3(gl_in[i].gl_Position);
	}

	center /= gl_in.length();
	
	gl_Position = projection * vec4(center, 1.0f);
	fragColor = faceNormalColor;
	EmitVertex();
	
	vec3 v0 = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position);
	vec3 v1 = vec3(gl_in[2].gl_Position - gl_in[1].gl_Position);
	vec3 faceNormal = normalize(cross(v1, v0));
	gl_Position = projection * vec4(center + (faceNormal * hairLength), 1.0f);
	fragColor = faceNormalColor;
	EmitVertex();

	EndPrimitive();
}

void main()
{
	outTriangleAsLines();

	if (hairState % 2 == 0) { outNormalLineHairs(); }
	if (hairState / 2 > 0) { outFaceNormalLineHairs(); }

}