#version 330 core

in vec2 vTexCoord;

uniform sampler2D uScreenTexture;

out vec4 fragColor;

void main()
{
    float depth = texture(uScreenTexture, vTexCoord).r;
    fragColor = vec4(vec3(depth), 1.0); 
}