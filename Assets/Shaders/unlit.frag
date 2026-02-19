#version 150

in vec2 fragTexCoord;

struct Material {
    vec3 diffuse;
    float alpha;
};

uniform Material material;
uniform sampler2D diffuseTexture;
uniform int hasDiffuseTexture;

out vec4 fragColor;

void main()
{
    vec3 color = material.diffuse;
    if(hasDiffuseTexture != 0)
        color *= texture(diffuseTexture, fragTexCoord).rgb;
    
    fragColor = vec4(color, material.alpha);
}
