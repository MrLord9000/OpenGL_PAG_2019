#version 330 core

layout (points) in;
layout (line_strip, max_vertices = 128) out;

uniform float radius;
uniform int sides;

in VS_OUT
{
	vec4 color;
	mat4 mvp;
} gs_in[];

out vec4 color;

float calcVertSin(float i)
{
	return sin(radians(360.0f / sides + (360.0f / sides) * i));
}

float calcVertCos(float i)
{
	return cos(radians(360.0f / sides + (360.0f / sides) * i));
}

void main()
{
	color = gs_in[0].color;
	for	(int i = 0; i < sides + 1; i++)
	{
		gl_Position = gl_in[0].gl_Position + gs_in[0].mvp * (vec4(calcVertSin(i) * radius, 0.0f, calcVertCos(i) * radius, 0.0f));    // 1
		EmitVertex();
	}
	EndPrimitive();
}