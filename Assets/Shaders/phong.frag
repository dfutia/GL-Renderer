#version 330 core

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vTexCoord;

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
uniform bool hasDiffuseTexture;

out vec4 fragColor;

void main()
{
    vec3 baseColor = material.diffuse;
    if (hasDiffuseTexture)
        baseColor *= texture(texture0, vTexCoord).rgb;
    
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(light.position - vWorldPos);
    vec3 viewDir = normalize(viewPos - vWorldPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    
    // Ambient
    vec3 ambient = material.ambient * baseColor;
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color * baseColor;
    
    // Specular
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * light.color * material.specular;
    
    vec3 result = (ambient + diffuse + specular) * light.intensity;
    fragColor = vec4(result, 1.0);
}