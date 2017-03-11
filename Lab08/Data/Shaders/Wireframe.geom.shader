#version 430 core

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

in vec3 geomNormalEye[];
in vec3 geomPosEye[];
out vec3 fragNormalEye;
out vec3 fragPosEye;
noperspective out vec3 fragEdgeDistance;

layout(location = 21) uniform mat4 viewportMatrix;
layout(location = 10) uniform vec3 tint;
out vec3 lightColor;

void main()
{
	vec3 points[gl_in.length()];
	float lengths[gl_in.length()];

	for (int i = 0; i < gl_in.length(); ++i)
	{
		points[i] = vec3(viewportMatrix * (gl_in[i].gl_Position / gl_in[i].gl_Position.w));
	}

	for (int i = 0; i < gl_in.length(); ++i)
	{
		int j = (i + 1) % gl_in.length(), k = (j + 1) % gl_in.length();
		lengths[i] = length(points[j] - points[k]);
	}

	for (int i = 0; i < gl_in.length(); ++i)
	{
		int j = (i + 1) % gl_in.length(), k = (j + 1) % gl_in.length();

		fragNormalEye = geomNormalEye[i];
		fragPosEye = geomPosEye[i];
		gl_Position = gl_in[i].gl_Position;
		lightColor = tint;
		float d = lengths[k] * sin(acos(((lengths[i] * lengths[i]) + (lengths[k] * lengths[k]) - (lengths[j] * lengths[j])) / (2.0f * lengths[i] * lengths[k])));
		fragEdgeDistance = vec3(i == 0 ? d : 0.0f, i == 1 ? d : 0.0f, i == 2 ? d : 0.0f);

		EmitVertex();
	}
	
	EndPrimitive();
}