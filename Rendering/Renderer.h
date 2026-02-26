#ifndef RENDERER_H
#define RENDERER_H

#include <memory>

#include "Graphics/GraphicsDevice.h"
#include "Graphics/FrameBuffer.h"
#include "ShaderLibrary.h"
#include "RenderQueue.h"

class Camera;
struct DirectionalLight;
struct Skybox;

class Renderer
{
public:
    Renderer(GraphicsDevice& graphics, ShaderLibrary& shaders);

    void SetCamera(Camera* camera) { this->camera = camera; }
    void SetLight(DirectionalLight* light) { this->light = light; }
    void SetSkybox(Skybox* skybox) { this->skybox = skybox; }
    void SetViewport(int width, int height);

    void Render(const RenderBatch& batch);

    bool shadowsEnabled = true;
private:
    void ShadowPass(const RenderBatch& batch);
    void ScenePass(const RenderBatch& batch);
    void SkyboxPass();
    void DrawRenderable(const Renderable& r, const glm::mat4& view, const glm::mat4& projection, bool shadowPass);

    GraphicsDevice& graphics;
    ShaderLibrary& shaders;

    Camera* camera = nullptr;
    DirectionalLight* light = nullptr;
    Skybox* skybox = nullptr;

    int viewportWidth = 1280;
    int viewportHeight = 720;

    std::unique_ptr<FrameBuffer> shadowMap;
};

#endif