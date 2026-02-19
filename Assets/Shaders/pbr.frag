#version 150

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;

struct Material {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
    float alpha;
};

struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;
uniform sampler2D albedoTexture;
uniform sampler2D metallicTexture;
uniform sampler2D roughnessTexture;
uniform sampler2D aoTexture;
uniform int hasAlbedoTexture;
uniform int hasMetallicTexture;
uniform int hasRoughnessTexture;
uniform int hasAOTexture;

out vec4 fragColor;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
    
    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{
    vec3 albedo = material.albedo;
    if(hasAlbedoTexture != 0)
        albedo *= pow(texture(albedoTexture, fragTexCoord).rgb, vec3(2.2));
    
    float metallic = material.metallic;
    if(hasMetallicTexture != 0)
        metallic *= texture(metallicTexture, fragTexCoord).r;
    
    float roughness = material.roughness;
    if(hasRoughnessTexture != 0)
        roughness *= texture(roughnessTexture, fragTexCoord).r;
    
    float ao = material.ao;
    if(hasAOTexture != 0)
        ao *= texture(aoTexture, fragTexCoord).r;
    
    vec3 N = normalize(fragNormal);
    vec3 V = normalize(viewPos - fragPos);
    
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    
    vec3 L = normalize(light.position - fragPos);
    vec3 H = normalize(V + L);
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = light.color * light.intensity * attenuation;
    
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;
    
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
    color = pow(color, vec3(1.0/2.2));
    
    fragColor = vec4(color, material.alpha);
}
