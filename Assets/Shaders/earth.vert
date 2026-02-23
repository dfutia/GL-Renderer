#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    float Height;
} vs_out;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;

uniform sampler2D heightMap;
uniform float heightMultiplier;

const float PI = 3.14159265359;

vec2 pointToUV(vec3 p)
{
    float lat = asin(clamp(p.y, -1.0, 1.0));
    float lon = atan(p.x, p.z);
    
    float u = (lon / PI) * 0.5 + 0.5;
    float v = (lat / PI) + 0.5;
    
    // Try flipping U (horizontal mirror)
    v = 1.0 - v;
    
    return vec2(u, v);
}

void main()
{
    vec3 unitPos = normalize(aPos);
    vec2 uv = pointToUV(unitPos);
    float height = texture(heightMap, uv).r;
    
    vec3 displaced = unitPos * (1.0 + height * heightMultiplier);
    
    vec3 fragPos = vec3(modelMatrix * vec4(displaced, 1.0));
    
    vs_out.FragPos = fragPos;
    vs_out.Normal = normalize(normalMatrix * unitPos);
    vs_out.TexCoords = uv;
    vs_out.Height = height;
    
    gl_Position = projectionMatrix * viewMatrix * vec4(fragPos, 1.0);
}