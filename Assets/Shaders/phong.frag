#version 330 core

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoord;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float alpha;
};

struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;
uniform sampler2D texture0;

out vec4 fragColor;

void main()
{
    vec4 texColor = texture(texture0, fragTexCoord);
    vec3 color = material.diffuse * texColor.rgb;
    
    // Ambient
    vec3 ambient = material.ambient * color;
    
    // Diffuse
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.color * color;
    
    // Specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * light.color * material.specular;
    
    vec3 result = (ambient + diffuse + specular) * light.intensity;
    fragColor = vec4(result, material.alpha * texColor.a);
}