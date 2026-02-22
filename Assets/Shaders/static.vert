#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightDir;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;
uniform mat4 lightSpaceMatrix;

uniform vec3 viewPos;
uniform vec3 lightDirection;

void main()
{
    vec3 fragPos = vec3(modelMatrix * vec4(aPos, 1.0));
    
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    T = normalize(T - dot(T, N) * N); // re-orthogonalize
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N)); // world to tangent space
    
    vs_out.FragPos = fragPos;
    vs_out.TexCoords = aTexCoords;
    vs_out.TangentLightDir = TBN * (-lightDirection);
    vs_out.TangentViewPos = TBN * viewPos;
    vs_out.TangentFragPos = TBN * fragPos;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
    
    gl_Position = projectionMatrix * viewMatrix * vec4(fragPos, 1.0);
}