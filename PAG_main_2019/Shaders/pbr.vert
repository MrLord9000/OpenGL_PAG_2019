#version 330 core

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec3 vertex_tangent;
layout (location = 3) in vec2 texture_coords;

out vec3 FragPos;
out vec3 Normal;
out vec2 Texcoord;
out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

mat3 calculateTBN(vec3 normal, vec3 tangent)
{
	vec3 T = normalize(vec3(model * vec4(tangent, 0.0f)));
	vec3 N = normalize(vec3(model * vec4(normal, 0.0f)));
	vec3 bitangent = cross(N, T);
	vec3 B = normalize(vec3(model * vec4(bitangent, 0.0f)));
	return mat3(T, B, N);
}

void main() 
{
	FragPos = vec3(model * vec4(vertex_position, 1.0f));
	Normal = mat3(transpose(inverse(model))) * vertex_normal;
	Texcoord = texture_coords;
	TBN = calculateTBN(vertex_normal, vertex_tangent);
	gl_Position = projection * view * model * vec4(vertex_position, 1.0f);
}