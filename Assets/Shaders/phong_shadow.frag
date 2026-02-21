#version 330 core

out vec4 FragColor;

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vTexCoord;
in vec4 vLightSpacePos;

struct Light
{
    vec3 direction;
    vec3 color;
    float intensity;
};

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Light light;
uniform Material material;
uniform vec3 viewPos;

uniform sampler2D texture0;
uniform sampler2D texture10;
uniform int hasDiffuseTexture;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0)
        return 0.0;

    float closestDepth = texture(texture10, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(-light.direction);
    vec3 viewDir = normalize(viewPos - vWorldPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // Ambient
    vec3 ambient = material.ambient;

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuse;

    // Specular
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * material.specular;

    // Shadow
    float shadow = ShadowCalculation(vLightSpacePos);

    // Combine
    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular) * light.color * light.intensity;

    // Texture
    vec3 texColor = vec3(1.0);
    if (hasDiffuseTexture == 1)
        texColor = texture(texture0, vTexCoord).rgb;

    FragColor = vec4(lighting * texColor, 1.0);
}