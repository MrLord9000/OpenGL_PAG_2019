#version 330 core

out vec4 frag_colour;

in vec3 colour;
in vec2 texcoord;

uniform vec4 ourColor;
uniform sampler2D ourTexture;

void main() 
{ 
	frag_colour = texture(ourTexture, texcoord) * ourColor; 
}