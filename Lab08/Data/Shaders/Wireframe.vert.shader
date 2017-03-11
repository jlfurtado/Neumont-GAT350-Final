#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 13) uniform mat4 modelToWorld;
layout(location = 16) uniform mat4 worldToView;
layout(location = 17) uniform mat4 projection;

out vec3 geomNormalEye;
out vec3 geomPosEye;

void main()
{
	geomNormalEye = normalize(vec3(transpose(inverse(modelToWorld)) * vec4(vertexNormal, 1.0f)));
	geomPosEye = vec3(modelToWorld * vec4(vertexPosition, 1.0f));

	// calculate eye space for vertex
	gl_Position = projection * worldToView * modelToWorld * vec4(vertexPosition, 1.0f);
}
