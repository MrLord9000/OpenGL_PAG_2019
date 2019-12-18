#version 330 core

out vec4 FragColor;

struct Material
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_occlusion1;
	float shininess;
};

struct DirectionalLight
{
	float intensity;

	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirectionalLight directionalLight;

struct PointLight
{
	float intensity;

	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	// Attenuation variables
	float constant;
	float linear;
	float quadratic;
};
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

struct SpotLight
{
	float intensity;

	vec3 position;
	vec3 direction;
	float cutoff;
	float outerCutoff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	// Attenuation variables
	float constant;
	float linear;
	float quadratic;

	sampler2D cookie;
};
#define NR_SPOT_LIGHTS 3
uniform SpotLight spotLights[NR_SPOT_LIGHTS];

in vec3 FragPos;
in vec3 Normal;
in vec2 Texcoord;

uniform vec3 viewPos;
uniform Material material;

vec3 calcDirectionalLight(DirectionalLight directionalLight, vec3 norm, vec3 viewDir);
vec3 calcPointLight(PointLight pointLight, vec3 norm, vec3 fragPos, vec3 viewDir);
vec3 calcSpotLight(SpotLight spotLight, vec3 norm, vec3 fragPos, vec3 viewDir);

void main() 
{ 
// Normalized normal vector for use in light calculations

	// Global helper variables
	vec3 viewDir, norm, result;
	float diff, spec;

	// Shared values
	result = vec3(0.0f, 0.0f, 0.0f);
	norm = normalize(Normal);
	viewDir = normalize(viewPos - FragPos);

// Directional light ------------------------------------------------------------------------
	result += calcDirectionalLight(directionalLight, norm, viewDir);
	
// Point light ---------------------------------------------------------------------------------
	for (int i = 0; i < NR_POINT_LIGHTS; i++)
		result += calcPointLight(pointLights[i], norm, FragPos, viewDir);

// Spot light -----------------------------------------------------------------------------------
	for (int i = 0; i < NR_SPOT_LIGHTS; i++)
		result += calcSpotLight(spotLights[i], norm, FragPos, viewDir);

	FragColor = vec4(result, 1.0f);
}


vec3 calcDirectionalLight(DirectionalLight directionalLight, vec3 norm, vec3 viewDir)
{
	// Ambient
	vec3 directionalAmbient = directionalLight.ambient * texture(material.texture_diffuse1, Texcoord).rgb;

	// Diffuse
	vec3 lightDir = normalize(-directionalLight.direction);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 directionalDiffuse = directionalLight.diffuse * diff * texture(material.texture_diffuse1, Texcoord).rgb;

	// Specular
	//vec3 reflectDir = reflect(-lightDir, norm);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(viewDir, halfwayDir), 0.0f), material.shininess);
	vec3 directionalSpecular = directionalLight.specular * spec * texture(material.texture_specular1, Texcoord).rgb;

	return (directionalAmbient + directionalDiffuse + directionalSpecular) * directionalLight.intensity;
}


vec3 calcPointLight(PointLight pointLight, vec3 norm, vec3 fragPos, vec3 viewDir)
{
	// Ambient
	vec3 pointAmbient = pointLight.ambient * texture(material.texture_diffuse1, Texcoord).rgb;

	// Diffuse
	vec3 lightDir = normalize(pointLight.position - fragPos);
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 pointDiffuse = pointLight.diffuse * diff * texture(material.texture_diffuse1, Texcoord).rgb;

	// Specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	vec3 pointSpecular = pointLight.specular * spec * texture(material.texture_specular1, Texcoord).rgb;

	// Attenuation value
	float distance = length(pointLight.position - fragPos);
	float attenuation = 1.0f / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));
	
	pointAmbient *= attenuation;
	pointDiffuse *= attenuation;
	pointSpecular *= attenuation;

	return (pointAmbient + pointDiffuse + pointSpecular) * pointLight.intensity;
}


vec3 calcSpotLight(SpotLight spotLight, vec3 norm, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(spotLight.position - fragPos);
	float theta = dot(lightDir, normalize(-spotLight.direction));
	float epsilon = spotLight.cutoff - spotLight.outerCutoff;
	float intensity = clamp((theta - spotLight.outerCutoff) / epsilon, 0.0f, 1.0f);

	// Skipping ambient

	// Diffuse
	float diff = max(dot(norm, lightDir), 0.0f);
	vec3 spotDiffuse = spotLight.diffuse * diff * texture(material.texture_diffuse1, Texcoord).rgb;// * texture(spotLight.cookie, fragPos.xy).rgb;

	// Specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
	vec3 spotSpecular = spotLight.specular * spec * texture(material.texture_specular1, Texcoord).rgb;

	// Attenuation value
	float distance = length(spotLight.position - fragPos);
	float attenuation = 1.0f / (spotLight.constant + spotLight.linear * distance + spotLight.quadratic * (distance * distance));

	spotDiffuse *= attenuation * intensity;
	spotSpecular *= attenuation * intensity;

	return (spotDiffuse + spotSpecular) * spotLight.intensity;
}