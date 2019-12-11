#version 330 core

out vec4 frag_colour;

in vec2 texcoord;
in vec3 normal;
in vec4 world;

uniform vec4 frag_color = {1.0f, 1.0f, 1.0f, 1.0f};

uniform sampler2D texture_diffuse0;
uniform sampler2D texture_diffuse1;

void main() 
{ 
	frag_colour = texture(texture_diffuse1, texcoord) * frag_color; 
}
