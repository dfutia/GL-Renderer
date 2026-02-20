#version 330 core

in vec2 fragTexCoord;

struct Material {
    vec3 diffuse;
    float alpha;
};

uniform Material material;
uniform sampler2D texture0;

out vec4 fragColor;

void main()
{
    vec4 texColor = texture(texture0, fragTexCoord);
    vec3 color = material.diffuse * texColor.rgb;
    fragColor = vec4(color, material.alpha * texColor.a);
}