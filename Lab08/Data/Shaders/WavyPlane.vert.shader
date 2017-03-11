#version 430 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 13) uniform mat4 modelToWorld;
layout(location = 16) uniform mat4 worldToView;
layout(location = 17) uniform mat4 projection;
layout(location = 32) uniform float amplitude;
layout(location = 33) uniform float wavenumber;
layout(location = 34) uniform float velocity;
layout(location = 35) uniform float time;

out vec3 geomNormalEye;
out vec3 geomPosEye;

void main()
{
	float theta = wavenumber*(vertexPosition.x - velocity * time);
	float height = amplitude * sin(theta);
	vec3 newPosition = vertexPosition + vec3(0.0f, height, 0.0f);

	float normalX = wavenumber * amplitude *cos(theta);
	vec3 normal = vec3(-normalX, 1.0f, 0.0f);

	geomNormalEye = normalize(transpose(inverse(mat3(modelToWorld))) * normalize(normal));
	geomPosEye = vec3(modelToWorld * vec4(newPosition, 1.0f));

	// calculate eye space for vertex
	gl_Position = projection * worldToView * modelToWorld * vec4(newPosition, 1.0f);
}
