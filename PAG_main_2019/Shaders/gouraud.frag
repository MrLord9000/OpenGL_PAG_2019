#version 330 core

out vec4 frag_colour;

in vec2 texcoord;
in vec4 world;
in vec4 shading;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_occlusion1;

void main() 
{ 
	frag_colour = texture(texture_diffuse1, texcoord) * texture(texture_occlusion1, texcoord) * shading; 
}
