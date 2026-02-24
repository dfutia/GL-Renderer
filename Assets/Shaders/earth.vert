#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangent;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 WorldPos;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;

uniform sampler2D heightMap;
uniform float heightMultiplier;
uniform float seaLevel;
uniform int useSeaLevel;

const float PI = 3.14159265359;

vec2 pointToUV(vec3 p)
{
    float lat = asin(clamp(p.y, -1.0, 1.0));
    float lon = atan(p.x, p.z);
    
    float u = (lon / PI) * 0.5 + 0.5;
    float v = (lat / PI) + 0.5;
    v = 1.0 - v;
    
    return vec2(u, v);
}

// Sample height at a point on the unit sphere
float getHeight(vec3 spherePos)
{
    vec2 uv = pointToUV(spherePos);
    float h = texture(heightMap, uv).r;
    if (useSeaLevel == 1 && h < seaLevel)
        h = seaLevel;
    return h;
}

void main()
{
    vec3 unitPos = normalize(aPos);
    vec2 uv = pointToUV(unitPos);
    float height = texture(heightMap, uv).r;
    
    float displacement = height;
    if (useSeaLevel == 1 && height < seaLevel)
    {
        displacement = seaLevel;
    }
    
    vec3 displaced = unitPos * (1.0 + displacement * heightMultiplier);
    
    // Calculate normal from heightmap using finite differences
    float eps = 0.001;  // Small offset for sampling neighbors
    
    // Get tangent vectors on the sphere
    vec3 tangent = normalize(cross(vec3(0.0, 1.0, 0.0), unitPos));
    if (length(tangent) < 0.01)
        tangent = normalize(cross(vec3(1.0, 0.0, 0.0), unitPos));
    vec3 bitangent = normalize(cross(unitPos, tangent));
    
    // Sample neighboring heights
    vec3 neighborX = normalize(unitPos + tangent * eps);
    vec3 neighborY = normalize(unitPos + bitangent * eps);
    
    float heightX = getHeight(neighborX);
    float heightY = getHeight(neighborY);
    
    vec3 displacedX = neighborX * (1.0 + heightX * heightMultiplier);
    vec3 displacedY = neighborY * (1.0 + heightY * heightMultiplier);
    
    // Calculate normal from the displaced positions
    vec3 edge1 = displacedX - displaced;
    vec3 edge2 = displacedY - displaced;
    vec3 calculatedNormal = normalize(cross(edge1, edge2));
    
    // Make sure normal points outward
    if (dot(calculatedNormal, unitPos) < 0.0)
        calculatedNormal = -calculatedNormal;
    
    FragPos = vec3(modelMatrix * vec4(displaced, 1.0));
    Normal = normalize(normalMatrix * calculatedNormal);
    TexCoords = uv;
    WorldPos = unitPos;
    
    gl_Position = projectionMatrix * viewMatrix * vec4(FragPos, 1.0);
}