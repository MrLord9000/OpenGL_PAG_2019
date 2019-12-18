#version 330 core

out vec4 FragColor;

struct Material
{
	sampler2D texture_albedo1;
	sampler2D texture_normal1;
	sampler2D texture_metallic1;
	sampler2D texture_roughness1;
	sampler2D texture_occlusion1;
};
uniform Material material;

struct DirectionalLight
{
	float intensity;
	vec3 direction;
	vec3 color;
};
uniform DirectionalLight directionalLight;

struct PointLight
{
	float intensity;
	float radius;
	vec3 position;
	vec3 color;
};
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

struct SpotLight
{
	float intensity;
	vec3 position;
	vec3 direction;
	vec3 color;

	float cutoff;
	float outerCutoff;

	sampler2D cookie;
};
#define NR_SPOT_LIGHTS 3
uniform SpotLight spotLights[NR_SPOT_LIGHTS];

in vec3 FragPos;
in vec3 Normal;
in vec2 Texcoord;
in mat3 TBN;

// Other uniforms
uniform vec3 viewPos;

// Shader constants
const float PI = 3.14159265359;

// Global shader variables
vec3 N, V, F0, albedo;
float metallic, roughness, ao;

float DistributionGGX(vec3 normal, vec3 halfway, float roughness);
float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

vec3 calcDirectionalLight(DirectionalLight light);
vec3 calcPointLight(PointLight light);
vec3 calcSpotLight(SpotLight light);

vec3 calculateNormal()
{
	vec3 normal = texture(material.texture_normal1, Texcoord).rgb; // Sample normal map texure
	normal = normalize(normal * 2.0f - 1.0f); // Transform from normalized 0.0 - 1.0 coordinates to -1.0 - 1.0
	normal = normalize(TBN * normal); // multiply by TBN to get world space normals
	return normal;
}

void main() 
{ 
// Sampling the textures for further use
	albedo = pow(texture(material.texture_albedo1, Texcoord).rgb, vec3(2.2f)); // Converting texture from sRGB gamma space to linear by using pow()
	metallic = texture(material.texture_metallic1, Texcoord).r;
	roughness = texture(material.texture_roughness1, Texcoord).r;
	ao = texture(material.texture_occlusion1, Texcoord).r;

// Properties shared between lights
	N = calculateNormal();
	V = normalize(viewPos - FragPos);

	// Reflectace calculation at normal incidence (looking perpendicularly)
	F0 = vec3(0.04f);
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0f);

// Directional light ------------------------------------------------------------------------
	Lo += calcDirectionalLight(directionalLight);
	
// Point light ---------------------------------------------------------------------------------
	for (int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		Lo += calcPointLight(pointLights[i]) * pointLights[i].intensity;
	}
	
// Spot light -----------------------------------------------------------------------------------
//	for (int i = 0; i < NR_SPOT_LIGHTS; i++)
//		result += calcSpotLight(spotLights[i], norm, FragPos, viewDir);

// Makeshift ambient lightning
	vec3 ambient = vec3(0.01f) * albedo * ao;
	vec3 color = ambient + Lo;

// HDR tonemapping
	color = color / (color + vec3(1.0f));
// Gamma correction
	color = pow(color, vec3(1.0f / 2.2f));

	FragColor = vec4(color, 1.0f);
}

vec3 calcPointLight(PointLight light)
{
	vec3 L = normalize(light.position - FragPos);
	vec3 H = normalize(V + L);
	float distance = length(light.position - FragPos);
	//float attenuation = 1.0f / (distance);
	float attenuation = pow(clamp(1 - pow((distance/light.radius), 4.0f), 0.0f, 1.0f), 2.0f) / (distance * distance + 1.0f);
	vec3 radiance = light.color * light.intensity * attenuation;

	// Cook-Torrance BDRF
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	vec3 F = fresnelSchlick(max(dot(H, V), 0.0f), F0);

	vec3 nominator = NDF * G * F;
	float denominator = 4 * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.0001f; // added 0.001f to prevent division by zero
	vec3 specular = nominator / denominator;

	// Calculate specular/albedo distribution
	vec3 kS = F; // kS equal to fresnel
	vec3 kD = vec3(1.0f) - kS; // Based on energy conservation get how much diffuse contributes to overall lightning
	kD *= 1.0f - metallic; // Multiply diffuse kD by inverse metalness because only non metals have diffuse light

	// Scale light by NdotL
	float NdotL = max(dot(N, L), 0.0f);

	//return vec3(radiance * NdotL * 7.0f);
	return (kD * albedo / PI + specular) * radiance * NdotL;
}

vec3 calcDirectionalLight(DirectionalLight light)
{
	vec3 L = normalize(light.direction);
	vec3 H = normalize(V + L);
	//float distance = length(light.position - FragPos);
	//float attenuation = 1.0f / (distance);
	//float attenuation = pow(clamp(1 - pow((distance/light.radius), 4.0f), 0.0f, 1.0f), 2.0f) / (distance * distance + 1.0f);
	vec3 radiance = light.color * light.intensity;

	// Cook-Torrance BDRF
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	vec3 F = fresnelSchlick(max(dot(H, V), 0.0f), F0);

	vec3 nominator = NDF * G * F;
	float denominator = 4 * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.0001f; // added 0.001f to prevent division by zero
	vec3 specular = nominator / denominator;

	// Calculate specular/albedo distribution
	vec3 kS = F; // kS equal to fresnel
	vec3 kD = vec3(1.0f) - kS; // Based on energy conservation get how much diffuse contributes to overall lightning
	kD *= 1.0f - metallic; // Multiply diffuse kD by inverse metalness because only non metals have diffuse light

	// Scale light by NdotL
	float NdotL = max(dot(N, L), 0.0f);

	//return vec3(radiance * NdotL);
	return (kD * albedo / PI + specular) * radiance * NdotL;
}

vec3 calcSpotLight(SpotLight light)
{
	vec3 L = normalize(light.position - FragPos);
	vec3 H = normalize(V + L);
	float distance = length(light.position - FragPos);
	//float attenuation = 1.0f / (distance);
	float attenuation = pow(clamp(1 - pow((distance/light.radius), 4.0f), 0.0f, 1.0f), 2.0f) / (distance * distance + 1.0f);
	vec3 radiance = light.color * light.intensity * attenuation;

	// Cook-Torrance BDRF
	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	vec3 F = fresnelSchlick(max(dot(H, V), 0.0f), F0);

	vec3 nominator = NDF * G * F;
	float denominator = 4 * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.0001f; // added 0.001f to prevent division by zero
	vec3 specular = nominator / denominator;

	// Calculate specular/albedo distribution
	vec3 kS = F; // kS equal to fresnel
	vec3 kD = vec3(1.0f) - kS; // Based on energy conservation get how much diffuse contributes to overall lightning
	kD *= 1.0f - metallic; // Multiply diffuse kD by inverse metalness because only non metals have diffuse light

	// Scale light by NdotL
	float NdotL = max(dot(N, L), 0.0f);

	//return vec3(radiance * NdotL * 7.0f);
	return (kD * albedo / PI + specular) * radiance * NdotL;
}

float DistributionGGX(vec3 normal, vec3 halfway, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(normal, halfway), 0.0f);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 *(a2 - 1.0f) + 1.0f);
	denom = PI * denom * denom;

	return nom / denom;
}

float GeometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness)
{
	float NdotV = max(dot(normal, viewDir), 0.0f);
	float NdotL = max(dot(normal, lightDir), 0.0f);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = roughness + 1.0f;
	float k = (r * r) / 8.0f;

	float nom = NdotV;
	float denom = NdotV * (1.0f - k) + k;

	return nom / denom;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	cosTheta = min(cosTheta, 1.0f);
	return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}

//vec3 calcSpotLight(SpotLight spotLight, vec3 norm, vec3 fragPos, vec3 viewDir)
//{
//	vec3 lightDir = normalize(spotLight.position - fragPos);
//	float theta = dot(lightDir, normalize(-spotLight.direction));
//	float epsilon = spotLight.cutoff - spotLight.outerCutoff;
//	float intensity = clamp((theta - spotLight.outerCutoff) / epsilon, 0.0f, 1.0f);
//
//	// Skipping ambient
//
//	// Diffuse
//	float diff = max(dot(norm, lightDir), 0.0f);
//	vec3 spotDiffuse = spotLight.diffuse * diff * texture(material.texture_diffuse1, Texcoord).rgb;// * texture(spotLight.cookie, fragPos.xy).rgb;
//
//	// Specular
//	vec3 reflectDir = reflect(-lightDir, norm);
//	float spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
//	vec3 spotSpecular = spotLight.specular * spec * texture(material.texture_specular1, Texcoord).rgb;
//
//	// Attenuation value
//	float distance = length(spotLight.position - fragPos);
//	float attenuation = 1.0f / (spotLight.constant + spotLight.linear * distance + spotLight.quadratic * (distance * distance));
//
//	spotDiffuse *= attenuation * intensity;
//	spotSpecular *= attenuation * intensity;
//
//	return (spotDiffuse + spotSpecular) * spotLight.intensity;
//}