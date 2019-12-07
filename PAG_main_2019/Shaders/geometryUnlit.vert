#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec4 color;

out VS_OUT
{
	vec4 color;
	mat4 mvp;
} vs_out;

void main()
{
	vs_out.mvp = projection * view * model;
	gl_Position = vs_out.mvp * vec4(position, 1.0f);
	vs_out.color = color;
}