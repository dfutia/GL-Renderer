// phong_shadow.frag
#version 330 core

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightDir;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec4 FragPosLightSpace;
} fs_in;

out vec4 FragColor;

uniform sampler2D texture0;  // diffuse
uniform sampler2D texture2;  // normal map
uniform sampler2D texture10; // shadow map

uniform int hasDiffuseTexture;
uniform int hasNormalMap;

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

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if (projCoords.z > 1.0)
        return 0.0;
    
    float closestDepth = texture(texture10, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float bias = 0.005;
    
    return currentDepth - bias > closestDepth ? 1.0 : 0.0;
}

void main()
{
    // Get normal from map or use default
    vec3 normal;
    if (hasNormalMap == 1)
    {
        normal = texture(texture2, fs_in.TexCoords).rgb;
        normal = normalize(normal * 2.0 - 1.0);
    }
    else
    {
        normal = vec3(0.0, 0.0, 1.0); // default tangent-space normal
    }
    
    // Get diffuse color
    vec3 diffuseColor = material.diffuse;
    if (hasDiffuseTexture == 1)
        diffuseColor = texture(texture0, fs_in.TexCoords).rgb;
    
    // Lighting in tangent space
    vec3 lightDir = normalize(fs_in.TangentLightDir);
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
    // Ambient
    vec3 ambient = material.ambient * diffuseColor;
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * diffuseColor * light.color * light.intensity;
    
    // Specular
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * material.specular * light.color * light.intensity;
    
    // Shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    
    vec3 result = ambient + (1.0 - shadow) * (diffuse + specular);
    FragColor = vec4(result, 1.0);
}