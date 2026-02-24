#include "Actors/Actor.h"
#include "Actors/ActorComponent.h"
#include "Actors/RenderComponent.h"

#include "Rendering/Material.h"
#include "Rendering/LineRenderer.h"

#include "Graphics/VertexArray.h"
#include "Graphics/VertexBuffer.h"

class EarthComponent : public ActorComponent
{
public:
    // Terrain settings
    float heightMultiplier = 0.1f;
    float seaLevel = 0.01f;
    bool useSeaLevel = true;
    glm::vec3 landColor = glm::vec3(0.2f, 0.6f, 0.1f);
    glm::vec3 mountainColor = glm::vec3(0.5f, 0.4f, 0.3f);
    glm::vec3 snowColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float snowHeight = 0.8f;
    float mountainHeight = 0.6f;
    int resolution = 800;

    // Water settings
    float waveScale1 = 0.2f;
    float waveScale2 = 0.15f;
    float waveSpeed1 = 0.03f;
    float waveSpeed2 = 0.02f;
    float waveStrength = 1.0f;
    float specularPower = 64.0f;
    float fresnelPower = 3.0f;
    glm::vec3 shallowColor = glm::vec3(0.1f, 0.4f, 0.7f);
    glm::vec3 deepColor = glm::vec3(0.0f, 0.1f, 0.4f);

    // Time tracking
    float totalTime = 0.0f;

    const char* GetName() const override { return "Earth"; }

    void OnUpdate(float deltaTime) override
    {
        totalTime += deltaTime;

        // Sync uniforms to RenderComponent every frame
        if (auto* render = owner->GetComponent<RenderComponent>())
        {
            render->SetUniform("heightMultiplier", heightMultiplier);
            render->SetUniform("seaLevel", seaLevel);
            render->SetUniform("useSeaLevel", useSeaLevel ? 1 : 0);
            //render->SetUniform("seaColor", seaColor);
            render->SetUniform("landColor", landColor);
            render->SetUniform("mountainColor", mountainColor);
            render->SetUniform("snowColor", snowColor);
            render->SetUniform("mountainHeight", mountainHeight);
            render->SetUniform("snowHeight", snowHeight);
            render->SetUniform("waveScale1", waveScale1);
            render->SetUniform("waveScale2", waveScale2);
            render->SetUniform("waveSpeed1", waveSpeed1);
            render->SetUniform("waveSpeed2", waveSpeed2);
            render->SetUniform("waveStrength", waveStrength);
            render->SetUniform("specularPower", specularPower);
            render->SetUniform("fresnelPower", fresnelPower);
            render->SetUniform("shallowColor", shallowColor);
            render->SetUniform("deepColor", deepColor);
            render->SetUniform("time", totalTime);
        }
    }

    void RegisterProperties(PropertyRegistry& registry) override
    {
        registry.Float("Height Multiplier", &heightMultiplier, 0.0f, 50.0f, 0.1f);
        registry.Float("Sea Level", &seaLevel, 0.0f, 1.0f, 0.01f);
        registry.Bool("Use Sea Level", &useSeaLevel);
        //registry.Color3("Sea Color", &seaColor);
        registry.Color3("Land Color", &landColor);
        registry.Color3("Mountain Color", &mountainColor);
        registry.Color3("Snow Color", &snowColor);
        registry.Float("Mountain Height", &mountainHeight, 0.0f, 1.0f, 0.01f);
        registry.Float("Snow Height", &snowHeight, 0.0f, 1.0f, 0.01f);
        registry.Int("Resolution", &resolution, 10, 200);
        registry.Float("Wave Scale 1", &waveScale1, 0.01f, 1.0f, 0.01f);
        registry.Float("Wave Scale 2", &waveScale2, 0.01f, 1.0f, 0.01f);
        registry.Float("Wave Speed 1", &waveSpeed1, 0.0f, 0.2f, 0.005f);
        registry.Float("Wave Speed 2", &waveSpeed2, 0.0f, 0.2f, 0.005f);
        registry.Float("Wave Strength", &waveStrength, 0.0f, 1.0f, 0.05f);
        registry.Float("Specular Power", &specularPower, 1.0f, 256.0f, 1.0f);
        registry.Float("Fresnel Power", &fresnelPower, 0.5f, 5.0f, 0.1f);
        registry.Color3("Shallow Color", &shallowColor);
        registry.Color3("Deep Color", &deepColor);
    }
};