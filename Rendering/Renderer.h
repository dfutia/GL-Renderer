#ifndef RENDERER_H
#define RENDERER_H

#include <memory>

#include "Graphics/GraphicsDevice.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/UniformBuffer.h"
#include "ShaderLibrary.h"
#include "RenderQueue.h"
#include "RenderData.h"
#include "Quad.h"

class Camera;
struct DirectionalLight;
struct Skybox;

class Renderer
{
public:
    Renderer(GraphicsDevice& device, ShaderLibrary& shaders,
        unsigned int width, unsigned int height, unsigned int msaaSamples = 4);

    void SetCamera(Camera* camera) { this->camera = camera; }
    void SetLight(DirectionalLight* light) { this->light = light; }
    void SetSkybox(Skybox* skybox) { this->skybox = skybox; }
    void SetShadowsEnabled(bool enabled) { shadowsEnabled = enabled; }
    void SetViewport(unsigned int width, unsigned int height);

    void Render(const RenderBatch& batch);
    const Texture& GetResult();

    void Blit(const Texture& source, ShaderProgram& shader);
    void Blit(const Texture& source, ShaderProgram& shader, FrameBuffer& target);

    void DrawSkybox(const Skybox& skybox, const glm::mat4& view, const glm::mat4& projection);
    void DrawScreenQuad(const ScreenQuad& quad, const ShaderProgram& shader, const Texture& texture);

    void ApplyMaterial(const ShaderProgram& shader, const Material& material);
    void ApplyLight(const ShaderProgram& shader, const DirectionalLight& light);

private:
    GraphicsDevice& device;
    ShaderLibrary& shaders;

    unsigned int viewportWidth;
    unsigned int viewportHeight;
    unsigned int msaaSamples;

    FrameBuffer msaaFBO;
    FrameBuffer resolveFBO;
    bool resolved = false;

    std::unique_ptr<FrameBuffer> shadowMap;
    UniformBuffer cameraUBO;
    UniformBuffer lightingUBO;

    Camera* camera = nullptr;
    DirectionalLight* light = nullptr;
    Skybox* skybox = nullptr;
    bool shadowsEnabled = true;

    ScreenQuad screenQuad;

    void ShadowPass(const RenderBatch& batch);
    void ScenePass(const RenderBatch& batch);
    void SkyboxPass();
    void DrawRenderable(const Renderable& r, const glm::mat4& view, const glm::mat4& projection, bool shadowPass);
    void UpdateCameraUBO();
    void UpdateLightingUBO();
};


#endif