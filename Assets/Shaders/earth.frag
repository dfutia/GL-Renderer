#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 WorldPos;

// Texture samplers
uniform sampler2D heightMap;
uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D specular;
uniform sampler2D waveNormal1;
uniform sampler2D waveNormal2;

// Terrain uniforms
uniform float heightMultiplier;
uniform float seaLevel;
uniform int useSeaLevel;

// Water uniforms
uniform float waveScale1;
uniform float waveScale2;
uniform float waveSpeed1;
uniform float waveSpeed2;
uniform float waveStrength;
uniform float specularPower;
uniform float fresnelPower;
uniform vec3 shallowColor;
uniform vec3 deepColor;
uniform float time;

// Lighting
uniform vec3 viewPos;
uniform vec3 lightDirection;
uniform vec3 lightColor;

// Triplanar normal sampling for water
vec3 triplanarNormal(sampler2D tex, vec3 worldPos, vec3 surfaceNormal, float scale, vec2 offset)
{
    vec3 blendWeights = abs(surfaceNormal);
    blendWeights = pow(blendWeights, vec3(4.0));
    blendWeights /= (blendWeights.x + blendWeights.y + blendWeights.z + 0.0001);
    
    vec3 xNormal = texture(tex, worldPos.yz * scale + offset).rgb * 2.0 - 1.0;
    vec3 yNormal = texture(tex, worldPos.xz * scale + offset).rgb * 2.0 - 1.0;
    vec3 zNormal = texture(tex, worldPos.xy * scale + offset).rgb * 2.0 - 1.0;
    
    vec3 xWorld = vec3(0.0, xNormal.y, xNormal.x);
    vec3 yWorld = vec3(yNormal.x, 0.0, yNormal.y);
    vec3 zWorld = vec3(zNormal.x, zNormal.y, 0.0);
    
    return normalize(
        xWorld * blendWeights.x + 
        yWorld * blendWeights.y + 
        zWorld * blendWeights.z +
        surfaceNormal
    );
}

void main()
{
    float height = texture(heightMap, TexCoords).r;
    
    vec3 surfaceNormal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 lightDir = normalize(-lightDirection);
    
    bool isWater = (useSeaLevel == 1) && (height < seaLevel);
    
    vec3 finalColor;
    
    if (isWater)
    {
        // === WATER RENDERING ===
        
        vec2 scroll1 = vec2(time * waveSpeed1, time * waveSpeed1 * 0.8);
        vec2 scroll2 = vec2(-time * waveSpeed2 * 0.7, time * waveSpeed2);
        
        vec3 waveNorm1 = triplanarNormal(waveNormal1, WorldPos, surfaceNormal, waveScale1, scroll1);
        vec3 waveNorm2 = triplanarNormal(waveNormal2, WorldPos, surfaceNormal, waveScale2, scroll2);
        
        vec3 waterNormal = normalize(mix(surfaceNormal, normalize(waveNorm1 + waveNorm2), waveStrength));
        
        float fresnel = pow(1.0 - max(dot(viewDir, waterNormal), 0.0), fresnelPower);
        
        vec3 waterColor = mix(deepColor, shallowColor, fresnel * 0.5 + 0.5);
        
        vec3 halfDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(waterNormal, halfDir), 0.0), specularPower);
        vec3 specularHighlight = vec3(1.0) * spec * fresnel;
        
        float diff = max(dot(waterNormal, lightDir), 0.0) * 0.3 + 0.7;
        
        finalColor = waterColor * diff + specularHighlight;
    }
    else
    {
        // === LAND RENDERING ===
        
        // Sample textures
        vec3 albedo = texture(diffuse, TexCoords).rgb;
        vec3 normalMap = texture(normal, TexCoords).rgb * 2.0 - 1.0;
        float specularMap = texture(specular, TexCoords).r;
        
        // Perturb normal with normal map (simplified tangent space)
        // For a sphere, we can approximate tangent space
        vec3 N = surfaceNormal;
        vec3 T = normalize(cross(vec3(0.0, 1.0, 0.0), N));
        vec3 B = cross(N, T);
        mat3 TBN = mat3(T, B, N);
        vec3 perturbedNormal = normalize(TBN * normalMap);
        
        // Lighting calculation
        float diff = max(dot(perturbedNormal, lightDir), 0.0);
        
        vec3 halfDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(perturbedNormal, halfDir), 0.0), 32.0) * specularMap;
        
        vec3 ambient = 0.15 * albedo;
        vec3 diffuseLight = diff * albedo;
        vec3 specularLight = spec * vec3(0.5);
        
        finalColor = ambient + diffuseLight + specularLight;
    }
    
    FragColor = vec4(finalColor, 1.0);
}