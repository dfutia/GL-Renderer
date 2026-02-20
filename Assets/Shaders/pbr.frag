#version 330 core

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vTexCoord;

const float PI = 3.14159265359;

struct Material
{
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
};

struct Light
{
    vec3 position;
    vec3 color;
    float intensity;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

uniform bool hasAlbedoMap;
uniform bool hasNormalMap;
uniform bool hasMetallicMap;
uniform bool hasRoughnessMap;
uniform bool hasAOMap;

out vec4 fragColor;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float denom = NdotH2 * (a2 - 1.0) + 1.0;
    denom = PI * denom * denom;
    
    return a2 / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{
    // Sample material properties
    vec3 albedo = material.albedo;
    float metallic = material.metallic;
    float roughness = material.roughness;
    float ao = material.ao;
    
    if (hasAlbedoMap)
        albedo *= pow(texture(albedoMap, vTexCoord).rgb, vec3(2.2));
    if (hasMetallicMap)
        metallic *= texture(metallicMap, vTexCoord).r;
    if (hasRoughnessMap)
        roughness *= texture(roughnessMap, vTexCoord).r;
    if (hasAOMap)
        ao *= texture(aoMap, vTexCoord).r;
    
    vec3 N = normalize(vNormal);
    vec3 V = normalize(viewPos - vWorldPos);
    
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    
    // Single light calculation
    vec3 L = normalize(light.position - vWorldPos);
    vec3 H = normalize(V + L);
    float distance = length(light.position - vWorldPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = light.color * light.intensity * attenuation;
    
    // Cook-Torrance BRDF
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);
    
    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);
    
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;
    
    float NdotL = max(dot(N, L), 0.0);
    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;
    
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;
    
    // HDR tonemapping
    color = color / (color + vec3(1.0));
    
    // Gamma correction
    color = pow(color, vec3(1.0 / 2.2));
    
    fragColor = vec4(color, 1.0);
}