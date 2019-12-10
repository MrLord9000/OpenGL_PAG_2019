#version 330 core

out vec4 frag_colour;

in vec2 texcoord;
in vec3 normal;
in vec4 world;

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_diffuse1;

void main() 
{ 
	// Temp ambient light
	vec4 ambient = vec4(0.1f, 0.1f, 0.4f, 1.0f);
	// Temp diffuse light
	vec3 norm = normalize(normal);
	vec3 diffuseColor = vec3(1.0f, 1.0f, 1.0f);
	vec3 lightPosition = vec3(-5.0f, 5.0f, 3.0f);
	vec3 lightDirection = normalize(lightPosition - vec3(world));
	vec3 diffuse = max(dot(normal, lightDirection), 0.0f) * diffuseColor;
	frag_colour = texture(texture_diffuse1, texcoord) * (ambient + vec4(diffuse, 1.0f)); 
}
