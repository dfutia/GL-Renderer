#version 330 core

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vTexCoord;
in vec4 vLightSpacePos;

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
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
uniform sampler2D texture0;
uniform sampler2D texture10;
uniform bool hasDiffuseTexture;

out vec4 fragColor;

float CalculateShadow(vec3 normal, vec3 lightDir)
{
    vec3 projCoords = vLightSpacePos.xyz / vLightSpacePos.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if (projCoords.z > 1.0)
        return 0.0;
    
    float currentDepth = projCoords.z;
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(texture10, 0);
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            float pcfDepth = texture(texture10, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    
    return shadow;
}

void main()
{
    vec3 baseColor = material.diffuse;
    if (hasDiffuseTexture)
        baseColor *= texture(texture0, vTexCoord).rgb;
    
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(light.position - vWorldPos);
    vec3 viewDir = normalize(viewPos - vWorldPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    
    vec3 ambient = material.ambient * baseColor;
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color * baseColor;
    
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * light.color * material.specular;
    
    float shadow = CalculateShadow(normal, lightDir);
    
    vec3 result = ambient + (1.0 - shadow) * (diffuse + specular) * light.intensity;
    fragColor = vec4(result, 1.0);
}