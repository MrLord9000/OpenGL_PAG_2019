#version 330 core

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 texture_coords;

out vec2 texcoord;
out vec4 world;
out vec4 shading;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 view_position;

uniform vec3 directional_vector;
uniform vec4 directional_color;

uniform vec4 ambient_color;

uniform vec3 point_0_position;
uniform vec4 point_0_color;

uniform float specular_intensity = 1.0f;
uniform float occlusion_intensity = 2.0f;

uniform sampler2D texture_specular1;

void main() 
{
	world = model * vec4(vertex_position, 1.0f);
	gl_Position = projection * view * world;
	texcoord = texture_coords;

	// Gouraud shading ----------------------------------
	// Calulating the normal matrix for world space
	mat3 normal_matrix = mat3(transpose(inverse(model)));
	vec3 normal = normal_matrix * vertex_normal;

	vec3 norm = normalize(normal);

	// Directional light
	vec3 directional = max(dot(normal, directional_vector), 0.0f) * vec3(directional_color);

	// Point light
	vec3 point_0_direction = normalize(point_0_position - vec3(world));
	vec3 diffuse = max(dot(normal, point_0_direction), 0.0f) * vec3(point_0_color);

	// Specular
	vec4 specularColor = texture(texture_specular1, texcoord) * specular_intensity;
	vec3 viewDir = normalize(view_position - vec3(world));
	vec3 reflectDir = reflect(-point_0_direction, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 256);
	vec3 specular = spec * vec3(specularColor); 

	shading = ambient_color + vec4(diffuse, 1.0f) + vec4(directional, 1.0f) + vec4(specular, 1.0f);
}