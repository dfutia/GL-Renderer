#version 330 core

in vec3 Normal;
in vec2 TexCoord;

out vec4 outColor;

uniform vec3 lightDir;
uniform sampler2D texture0;

uniform struct {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
} material;

uniform bool hasTexture;

void main()
{
    float diff = max(dot(normalize(Normal), normalize(lightDir)), 0.0) * 0.7 + 0.3;
    
    vec4 baseColor = texture(texture0, TexCoord);
    if (baseColor.a < 0.01)
        baseColor = vec4(material.diffuse, 1.0);
    
    outColor = baseColor * diff;
}