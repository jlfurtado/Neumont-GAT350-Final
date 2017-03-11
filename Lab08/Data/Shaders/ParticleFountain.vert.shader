#version 430 core

layout(location = 0) in vec3 initialVelocity;
layout(location = 1) in vec2 spawnTime;

out float trans;

layout(location = 35) uniform float time;
layout(location = 33) uniform vec3  uGravity;
layout(location = 34) uniform float uParticleLifetime;
layout(location = 13) uniform mat4 modelToWorld;
layout(location = 16) uniform mat4 worldToView;

void main()
{
	vec3 pos =  vec3(0.0f);
	trans = 0.0f;

	if (time > spawnTime.x)
	{
		float t = time - spawnTime.x;
		if (t < uParticleLifetime)
		{
			pos = initialVelocity * t + mat3(inverse(modelToWorld)) * uGravity * t * t;
			trans = (1.0f - (t / uParticleLifetime));
		}
	}

	gl_Position = worldToView * modelToWorld * vec4(pos, 1.0);
}
