#version 330 core

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 texture_coords;

out vec3 FragPos;
out vec3 Normal;
out vec2 Texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() 
{
	FragPos = vec3(model * vec4(vertex_position, 1.0f));
	Normal = mat3(transpose(inverse(model))) * vertex_normal;
	Texcoord = texture_coords;
	gl_Position = projection * view * model * vec4(vertex_position, 1.0f);
}