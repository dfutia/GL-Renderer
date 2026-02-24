#version 330 core

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vTexCoord;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 direction;
    vec3 color;
    float intensity;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;
uniform sampler2D diffuse;  // Match the name in BindMaterial
uniform int hasDiffuseTexture;

out vec4 fragColor;

void main()
{
    vec3 baseColor = material.diffuse;
    if (hasDiffuseTexture == 1)
        baseColor *= texture(diffuse, vTexCoord).rgb;
    
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(-light.direction);
    vec3 viewDir = normalize(viewPos - vWorldPos);
    vec3 halfDir = normalize(lightDir + viewDir);
    
    vec3 ambient = material.ambient * baseColor;
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuseVec = diff * light.color * baseColor;
    float spec = pow(max(dot(normal, halfDir), 0.0), material.shininess);
    vec3 specular = spec * light.color * material.specular;
    
    vec3 result = (ambient + diffuseVec + specular) * light.intensity;
    fragColor = vec4(result, 1.0);
}