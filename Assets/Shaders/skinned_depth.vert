#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in ivec4 aBoneIDs;
layout(location = 4) in vec4 aBoneWeights;
layout(location = 5) in vec3 aTangent;

const int MAX_BONES = 100;

uniform mat4 lightSpaceMatrix;
uniform mat4 modelMatrix;
uniform mat4 bones[MAX_BONES];

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
    gl_Position = lightSpaceMatrix * modelMatrix * skinnedPos;
}