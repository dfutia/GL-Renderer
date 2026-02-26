#version 420 core

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightDir;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec4 FragPosLightSpace;
} fs_in;

out vec4 FragColor;

layout(std140, binding = 1) uniform LightingData
{
    vec4 lightDirection;
    vec4 lightAmbient;
    vec4 lightDiffuse;
    vec4 lightSpecular;
    mat4 lightSpaceMatrix;
    vec4 shadowParams;  // x = enabled, y = bias
};

uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D shadowMap;

uniform int hasDiffuseTexture;
uniform int hasNormalMap;
uniform int hasShadowMap;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if (projCoords.z > 1.0)
        return 0.0;
    
    float currentDepth = projCoords.z;
    float bias = shadowParams.y;
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    
    for (int x = -2; x <= 2; x++)
    {
        for (int y = -2; y <= 2; y++)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 25.0;
    
    return shadow;
}

void main()
{
    vec3 N;
    if (hasNormalMap == 1)
    {
        N = texture(normal, fs_in.TexCoords).rgb;
        N = normalize(N * 2.0 - 1.0);
    }
    else
    {
        N = vec3(0.0, 0.0, 1.0);
    }
    
    vec3 diffuseColor = material.diffuse;
    if (hasDiffuseTexture == 1)
        diffuseColor = texture(diffuse, fs_in.TexCoords).rgb;
    
    vec3 lightDir = normalize(fs_in.TangentLightDir);
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
    vec3 ambient = lightAmbient.rgb * material.ambient * diffuseColor;
    
    float diff = max(dot(N, lightDir), 0.0);
    vec3 diffuseVec = diff * diffuseColor * lightDiffuse.rgb;
    
    float spec = pow(max(dot(N, halfwayDir), 0.0), material.shininess);
    vec3 specularVec = spec * material.specular * lightSpecular.rgb;
    
    float shadow = 0.0;
    if (hasShadowMap == 1 && shadowParams.x > 0.5)
        shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    
    vec3 result = ambient + (1.0 - shadow) * (diffuseVec + specularVec);
    FragColor = vec4(result, 1.0);
}