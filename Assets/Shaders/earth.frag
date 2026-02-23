#version 330 core

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    float Height;
} fs_in;

out vec4 FragColor;

uniform vec3 seaColor;
uniform vec3 landColor;
uniform vec3 mountainColor;
uniform vec3 snowColor;

uniform float seaLevel;
uniform float mountainHeight;
uniform float snowHeight;
uniform int useSeaLevel;

uniform vec3 lightDirection;
uniform vec3 viewPos;

void main()
{
    float height = fs_in.Height;
    vec3 color;
    
    if (useSeaLevel == 1 && height < seaLevel)
    {
        color = seaColor;
    }
    else
    {
        // Normalize height above sea level
        float landHeight = (height - seaLevel) / (1.0 - seaLevel);
        
        if (landHeight > snowHeight)
        {
            color = snowColor;
        }
        else if (landHeight > mountainHeight)
        {
            float t = (landHeight - mountainHeight) / (snowHeight - mountainHeight);
            color = mix(mountainColor, snowColor, t);
        }
        else
        {
            float t = landHeight / mountainHeight;
            color = mix(landColor, mountainColor, t);
        }
    }
    
    // Simple lighting
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(-lightDirection);
    
    float ambient = 0.2;
    float diff = max(dot(normal, lightDir), 0.0);
    
    vec3 result = (ambient + diff) * color;
    
    FragColor = vec4(result, 1.0);
}