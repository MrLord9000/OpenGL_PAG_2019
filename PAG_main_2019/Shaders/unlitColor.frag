#version 330 core

out vec4 frag_colour;

uniform vec4 color = {1.0f, 1.0f, 1.0f, 1.0f};

void main() 
{ 
	frag_colour = color; 
}
