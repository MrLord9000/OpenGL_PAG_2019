#version 330 core

out vec4 frag_color;

in vec2 texcoord;
in vec3 normal;
in vec4 view_space_pos;

// Light positions transformed into view space
in vec3 point_0_pos;
uniform vec4 point_0_color;

uniform vec3 directional_vector;
uniform vec4 directional_color;
uniform vec4 ambient_color;

uniform float specular_intensity = 1.0f;
uniform float occlusion_intensity = 2.0f;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_occlusion1;

void main() 
{ 
	vec3 norm = normalize(normal);

	// Directional light
	vec3 directional = max(dot(normal, directional_vector), 0.0f) * vec3(directional_color);

	// Point light
	vec3 point_0_direction = normalize(point_0_pos - vec3(view_space_pos));
	vec3 diffuse = max(dot(norm, point_0_direction), 0.0f) * vec3(point_0_color);

	// Specular
	vec4 specularColor = texture(texture_specular1, texcoord) * specular_intensity;
	vec3 viewDir = normalize(vec3(view_space_pos));// Viewer is always at 0,0,0 so view dir is 0,0,0
	vec3 reflectDir = reflect(-point_0_direction, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 256);
	vec3 specular = spec * vec3(specularColor); 

	frag_color = texture(texture_diffuse1, texcoord) * texture(texture_occlusion1, texcoord) * ((ambient_color) + vec4(diffuse, 1.0f) + vec4(directional, 1.0f) + vec4(specular, 1.0f)); 
}
