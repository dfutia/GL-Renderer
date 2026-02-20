#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;

out vec3 vWorldPos;
out vec3 vNormal;
out vec2 vTexCoord;

void main()
{
    vec4 worldPos = modelMatrix * vec4(aPosition, 1.0);
    vWorldPos = worldPos.xyz;
    vNormal = normalize(normalMatrix * aNormal);
    vTexCoord = aTexCoord;
    
    gl_Position = projectionMatrix * viewMatrix * worldPos;
}