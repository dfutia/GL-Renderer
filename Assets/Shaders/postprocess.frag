#version 330 core

in vec2 vTexCoord;

uniform sampler2D uScreenTexture;

out vec4 fragColor;

void main()
{
    vec3 color = texture(uScreenTexture, vTexCoord).rgb;
    fragColor = vec4(color, 1.0);
}