#version 330 core

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 texture_coords;

out vec2 texcoord;
out vec3 normal;
out vec4 view_space_pos;
out vec3 point_0_pos;

uniform vec3 point_0_position; // Point light position to transform into view space

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() 
{
	texcoord = texture_coords;
	view_space_pos = view * model * vec4(vertex_position, 1.0f);
	normal = mat3(transpose(inverse(model))) * vertex_normal;

	// Transform lights into view space
	point_0_pos = vec3(view * vec4(point_0_position, 1.0f));

	gl_Position = projection * view_space_pos;
}