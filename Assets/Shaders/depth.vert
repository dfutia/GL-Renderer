#version 420 core

layout(location = 0) in vec3 aPosition;

#ifdef SKINNED
layout(location = 3) in ivec4 aBoneIDs;
layout(location = 4) in vec4 aBoneWeights;

const int MAX_BONES = 100;
uniform mat4 bones[MAX_BONES];
#endif

uniform mat4 modelMatrix;
uniform mat4 lightSpaceMatrix;

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
    gl_Position = lightSpaceMatrix * modelMatrix * skinnedPos;
#else
    gl_Position = lightSpaceMatrix * modelMatrix * vec4(aPosition, 1.0);
#endif
}