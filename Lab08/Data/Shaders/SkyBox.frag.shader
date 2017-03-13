#version 430 core

in vec3 fPos;
in vec2 UV;
out vec4 fColor;

layout(location = 15) uniform sampler2D textureSampler;
layout(location = 3) uniform vec2 randomValue;
layout(location = 1) uniform float repeatScale;
layout(location = 2) uniform vec3 rgb;
layout(location = 4) uniform int numIterations;

void main()
{
	float repeatScaleOverTwo = repeatScale/2.0f;

	int xI = int(floor(UV.x / repeatScaleOverTwo));
	int zI = int(floor(UV.y / repeatScaleOverTwo));
	vec2 z = (mod(UV, repeatScale)-vec2(repeatScaleOverTwo, repeatScaleOverTwo));

	if (mod(floor(xI + zI), 2) == 0) { z = z *mat2(-1, 0, 0, 1); } // rotate 180
	
	int i;
	float f2;
	for (i = 0; i<numIterations; i++) {
		float x = (z.x * z.x - z.y * z.y) + randomValue.x;
		float y = (z.y * z.x + z.x * z.y) + randomValue.y;

		f2 = x * x + y * y;
		if (f2 > 4.0) { break; }

		z.x = x;
		z.y = y;
	}
	
	if (i == numIterations) { discard; }

	float f = (i+f2) / 100.0f;
	fColor = texture2D(textureSampler, vec2(f, 0.5f));
}