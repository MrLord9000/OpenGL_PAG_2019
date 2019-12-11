#version 330 core

out vec4 FragColor;

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	float shininess;
};

struct Light
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 Texcoord;

uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main() 
{ 
	// Ambient
	vec3 ambient = light.ambient * texture(material.texture_diffuse1, Texcoord).rgb;

	// Diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, Texcoord).rgb;

	// Specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	vec3 specular = light.specular * spec * texture(material.texture_specular1, Texcoord).rgb;

	vec3 result = ambient + diffuse + specular;
	
	FragColor = vec4(result, 1.0f);
}
