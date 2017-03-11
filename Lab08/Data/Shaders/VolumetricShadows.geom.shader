#version 430 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

in ShadowBlock
{
	vec3 lightColor;
	vec3 fragNormWorld;
	vec3 fragPosWorld;
} inData[];

out ShadowBlock
{
	vec3 lightColor;
	vec3 fragNormWorld;
	vec3 fragPosWorld;
} outData;

layout(location = 32) uniform vec3 eyeLightPos;
layout(location = 17) uniform mat4 projection;

subroutine void GeomRenderPassType();
subroutine uniform GeomRenderPassType geomRenderPass;

bool FacesLight(vec3 a, vec3 b, vec3 c) // verts in eye space
{
	vec3 n = cross(b - a, c - a);
	vec3 da = eyeLightPos.xyz - a;
	vec3 db = eyeLightPos.xyz - b;
	vec3 dc = eyeLightPos.xyz - c;
		
	// repeat for others
	return ((dot(n, da) > 0) || (dot(n, db) > 0) || (dot(n, dc) > 0));
}

void EmitEdgeQuad(vec3 a, vec3 b) // ab defines the edge, verts in eye space
{
	gl_Position = projection * vec4(a, 1);
	EmitVertex();

	gl_Position = projection * vec4(a - eyeLightPos.xyz, 0);// w == 0 !!
	EmitVertex();

	gl_Position = projection * vec4(b, 1);
	EmitVertex();

	gl_Position = projection * vec4(b - eyeLightPos.xyz, 0);// w == 0 !!
	EmitVertex();

	EndPrimitive();
}

subroutine(GeomRenderPassType)
void DoEdges()
{
	if (FacesLight(vec3(gl_in[0].gl_Position), vec3(gl_in[1].gl_Position), vec3(gl_in[2].gl_Position)))
	{
		EmitEdgeQuad(vec3(gl_in[0].gl_Position), vec3(gl_in[1].gl_Position));
		EmitEdgeQuad(vec3(gl_in[1].gl_Position), vec3(gl_in[2].gl_Position));
		EmitEdgeQuad(vec3(gl_in[2].gl_Position), vec3(gl_in[0].gl_Position));
	}
}

subroutine(GeomRenderPassType)
void PassThrough()
{
	for (int i = 0; i < gl_in.length(); ++i)
	{
		gl_Position = projection * gl_in[i].gl_Position;
		outData.lightColor = inData[i].lightColor;
		outData.fragNormWorld = inData[i].fragNormWorld;
		outData.fragPosWorld = inData[i].fragPosWorld;
		EmitVertex();
	}

	EndPrimitive();
}

void main()
{
	geomRenderPass();
}
