#version 330 core

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 texture_coords;

out vec2 texcoord;
out vec3 normal;
out vec4 world;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() 
{
	world = model * vec4(vertex_position, 1.0f);
	gl_Position = projection * view * world;
	texcoord = texture_coords;
	normal = vertex_normal;
}