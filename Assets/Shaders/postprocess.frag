#version 330 core

in vec2 fragTexCoord;
out vec4 fragColor;

uniform sampler2D screenTexture;

void main()
{
    vec3 color = texture(screenTexture, fragTexCoord).rgb;
    fragColor = vec4(1.0 - color, 1.0); // invert
}