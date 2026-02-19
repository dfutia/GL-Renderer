#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in ivec4 boneIDs;
layout(location = 4) in vec4 boneWeights;

out vec3 Normal;
out vec2 TexCoord;

uniform mat4 mvp;

const int MAX_BONES = 100;
uniform mat4 bones[MAX_BONES];
uniform int useSkinning;

void main()
{
    vec4 skinnedPosition;
    vec3 skinnedNormal;

    if (useSkinning == 1)
    {
        mat4 skin = mat4(0.0);

        for (int i = 0; i < 4; i++)
        {
            if (boneIDs[i] >= 0)
                skin += bones[boneIDs[i]] * boneWeights[i];
        }

        if (skin == mat4(0.0))
            skin = mat4(1.0);

        skinnedPosition = skin * vec4(position, 1.0);
        skinnedNormal = mat3(skin) * normal;
    }
    else
    {
        skinnedPosition = vec4(position, 1.0);
        skinnedNormal = normal;
    }

    Normal = skinnedNormal;
    TexCoord = texcoord;
    gl_Position = mvp * skinnedPosition;
}