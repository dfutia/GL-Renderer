#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in ivec4 aBoneIDs;
layout(location = 4) in vec4 aBoneWeights;

const int MAX_BONES = 100;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;
uniform mat4 bones[MAX_BONES];

out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vTexCoord;

void main()
{
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
    
    vec4 worldPos = modelMatrix * skinnedPos;
    vWorldPos = worldPos.xyz;
    vNormal = normalize(normalMatrix * skinnedNormal);
    vTexCoord = aTexCoord;
    
    gl_Position = projectionMatrix * viewMatrix * worldPos;
}