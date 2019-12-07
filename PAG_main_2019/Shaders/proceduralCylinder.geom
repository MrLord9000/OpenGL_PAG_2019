#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 128) out;

uniform float height;
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

void BuildCylinder(vec4 position)
{    
	color = gs_in[0].color;
	float hh = height / 2.0f;
	for (int i = 0; i < sides; i = i + 2)
	{
		float vertSin0 = calcVertSin(i) * radius;
		float vertSin1 = calcVertSin(i + 1) * radius;
		float vertSin2 = calcVertSin(i + 2) * radius;
		float vertCos0 = calcVertCos(i) * radius;
		float vertCos1 = calcVertCos(i + 1) * radius;
		float vertCos2 = calcVertCos(i + 2) * radius;
		// Segment even =======================================================================
		gl_Position = position + gs_in[0].mvp * (vec4(0.0f, -hh, 0.0f, 0.0f));    // 0
		EmitVertex();  
		gl_Position = position + gs_in[0].mvp * (vec4(vertSin0, -hh, vertCos0, 0.0f));    // 1
		EmitVertex();   
		gl_Position = position + gs_in[0].mvp * (vec4(vertSin1, -hh, vertCos1, 0.0f));    // 2
		EmitVertex();
		gl_Position = position + gs_in[0].mvp * (vec4(vertSin0, hh, vertCos0, 0.0f));    // 3
		EmitVertex();
		gl_Position = position + gs_in[0].mvp * (vec4(vertSin1, hh, vertCos1, 0.0f));    // 4
		EmitVertex();
		gl_Position = position + gs_in[0].mvp * (vec4(0.0f, hh, 0.0f, 0.0f));    // 5
		EmitVertex();
		gl_Position = position + gs_in[0].mvp * (vec4(vertSin1, hh, vertCos1, 0.0f));   // 6
		EmitVertex();
		// Segment odd ========================================================================
		gl_Position = position + gs_in[0].mvp * (vec4(vertSin2, hh, vertCos2, 0.0f));   // 7
		EmitVertex();
		gl_Position = position + gs_in[0].mvp * (vec4(vertSin1, -hh, vertCos1, 0.0f));   // 8
		EmitVertex();
		gl_Position = position + gs_in[0].mvp * (vec4(vertSin2, -hh, vertCos2, 0.0f));   // 9
		EmitVertex();
		gl_Position = position + gs_in[0].mvp * (vec4(0.0f, -hh, 0.0f, 0.0f));    // 10
		EmitVertex();
		gl_Position = position + gs_in[0].mvp * (vec4(vertSin2, -hh, vertCos2, 0.0f));   // 11
		EmitVertex();

		EndPrimitive();
	}
}

void main()
{
	BuildCylinder(gl_in[0].gl_Position);
}