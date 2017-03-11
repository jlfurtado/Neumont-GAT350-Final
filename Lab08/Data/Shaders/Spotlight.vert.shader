#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

out vec3 lightColor;
out vec3 fragNormWorld;
out vec3 fragPosWorld;

layout(location = 13) uniform mat4 modelToWorld;
layout(location = 16) uniform mat4 worldToView;
layout(location = 17) uniform mat4 projection;
layout(location = 10) uniform vec3 tint;

void main()
{
	lightColor = tint;
	fragNormWorld = mat3(transpose(inverse(modelToWorld))) * vertexNormal;
	fragPosWorld = vec3(modelToWorld * vec4(vertexPosition, 1.0f));

	// calculate position of vertex
	gl_Position = projection * worldToView * modelToWorld * vec4(vertexPosition, 1.0f);
}
