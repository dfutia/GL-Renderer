#version 420 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

#ifdef SKINNED
layout(location = 3) in ivec4 aBoneIDs;
layout(location = 4) in vec4 aBoneWeights;
layout(location = 5) in vec3 aTangent;
#else
layout(location = 3) in vec3 aTangent;
#endif

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightDir;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec4 FragPosLightSpace;
} vs_out;

layout(std140, binding = 0) uniform CameraData
{
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 viewProjectionMatrix;
    vec4 viewPos;
};

layout(std140, binding = 1) uniform LightingData
{
    vec4 lightDirection;
    vec4 lightAmbient;
    vec4 lightDiffuse;
    vec4 lightSpecular;
    mat4 lightSpaceMatrix;
    vec4 shadowParams;
};

uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

#ifdef SKINNED
const int MAX_BONES = 100;
uniform mat4 bones[MAX_BONES];
#endif

void main()
{
#ifdef SKINNED
    mat4 skinMatrix = mat4(0.0);
    for (int i = 0; i < 4; i++)
    {
        if (aBoneIDs[i] >= 0)
            skinMatrix += bones[aBoneIDs[i]] * aBoneWeights[i];
    }
    if (skinMatrix == mat4(0.0))
        skinMatrix = mat4(1.0);

    vec4 skinnedPos = skinMatrix * vec4(aPosition, 1.0);
    vec3 skinnedNormal = mat3(skinMatrix) * aNormal;
    vec3 skinnedTangent = mat3(skinMatrix) * aTangent;

    vec4 worldPos = modelMatrix * skinnedPos;
    vec3 N = normalize(normalMatrix * skinnedNormal);
    vec3 T = normalize(normalMatrix * skinnedTangent);
#else
    vec4 worldPos = modelMatrix * vec4(aPosition, 1.0);
    vec3 N = normalize(normalMatrix * aNormal);
    vec3 T = normalize(normalMatrix * aTangent);
#endif

    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    mat3 TBN = transpose(mat3(T, B, N));

    vs_out.FragPos = worldPos.xyz;
    vs_out.TexCoords = aTexCoords;
    vs_out.TangentLightDir = TBN * (-lightDirection.xyz);
    vs_out.TangentViewPos = TBN * viewPos.xyz;
    vs_out.TangentFragPos = TBN * worldPos.xyz;
    vs_out.FragPosLightSpace = lightSpaceMatrix * worldPos;

    gl_Position = viewProjectionMatrix * worldPos;
}