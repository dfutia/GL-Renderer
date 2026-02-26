#version 420 core

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vTexCoord;

uniform vec4 color;
uniform sampler2D texture0;
uniform bool hasDiffuseTexture;

out vec4 fragColor;

void main()
{
    vec4 baseColor = color;
    
    if (hasDiffuseTexture)
        baseColor *= texture(texture0, vTexCoord);
    
    fragColor = baseColor;
}