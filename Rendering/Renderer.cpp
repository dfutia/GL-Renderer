#include "PCH.h"
#include "Renderer.h"

#include "Camera.h"
#include "Light.h"
#include "Skybox.h"

Renderer::Renderer(GraphicsDevice& graphics, ShaderLibrary& shaders)
    : graphics(graphics), shaders(shaders)
    , cameraUBO(sizeof(CameraUBO))
    , lightingUBO(sizeof(LightingUBO))
{
    cameraUBO.Bind(0);
    lightingUBO.Bind(1);
}

void Renderer::SetViewport(int width, int height)
{
    viewportWidth = width;
    viewportHeight = height;
}

void Renderer::Render(const RenderBatch& batch)
{
    if (!camera || !light)
        return;

    if (shadowsEnabled && !shadowMap)
    {
        shadowMap = std::make_unique<FrameBuffer>(
            light->shadowWidth,
            light->shadowHeight,
            FrameBuffer::DepthOnly, 0, 24
        );
    }

    if (shadowsEnabled)
    {
        ShadowPass(batch);
    }

    UpdateCameraUBO();
    UpdateLightingUBO();

    ScenePass(batch);

    if (skybox)
    {
        SkyboxPass();
    }
}

void Renderer::ShadowPass(const RenderBatch& batch)
{
    glm::mat4 lightProjection = glm::ortho(
        -light->shadowOrthoSize, light->shadowOrthoSize,
        -light->shadowOrthoSize, light->shadowOrthoSize,
        light->shadowNearPlane, light->shadowFarPlane
    );
    glm::vec3 lightPos = -light->direction * light->shadowDistance;
    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    light->lightSpaceMatrix = lightProjection * lightView;
    light->shadowMap = &shadowMap->GetDepthTexture();

    graphics.SetViewport(0, 0, light->shadowWidth, light->shadowHeight);
    graphics.BindFrameBuffer(*shadowMap);
    graphics.Clear(false, true, false);
    graphics.SetDepthTest(true);

    for (const auto& r : batch.GetRenderables())
    {
        if (r.queue >= RenderQueue::Transparent)
            continue;
        if (!r.castsShadows)
            continue;

        DrawRenderable(r, lightView, lightProjection, true);
    }
}

void Renderer::ScenePass(const RenderBatch& batch)
{
    graphics.BindFrameBuffer(0);
    graphics.SetViewport(0, 0, viewportWidth, viewportHeight);
    graphics.SetClearColor(0.0f, 0.0f, 0.0f);
    graphics.Clear(true, true, false);
    graphics.SetDepthTest(true);

    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection = camera->GetProjectionMatrix();

    int currentQueue = -1;

    for (const auto& r : batch.GetRenderables())
    {
        if (r.queue != currentQueue)
        {
            currentQueue = r.queue;

            if (currentQueue >= RenderQueue::Transparent)
            {
                graphics.SetBlending(true);
                graphics.SetDepthWrite(false);
            }
            else
            {
                graphics.SetBlending(false);
                graphics.SetDepthWrite(true);
            }
        }

        DrawRenderable(r, view, projection, false);
    }

    graphics.SetBlending(false);
    graphics.SetDepthWrite(true);
}

void Renderer::SkyboxPass()
{
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection = camera->GetProjectionMatrix();
    graphics.DrawSkybox(*skybox, view, projection);
}

void Renderer::DrawRenderable(const Renderable& r, const glm::mat4& view,
    const glm::mat4& projection, bool shadowPass)
{
    std::string shaderName = shadowPass ? r.shadowShader : r.shader;
    ShaderProgram* shader = shaders.Get(shaderName);
    if (!shader)
        return;

    graphics.BindShader(*shader);

    if (shadowPass)
    {
        graphics.SetUniform("lightSpaceMatrix", light->lightSpaceMatrix);
        graphics.SetUniform("modelMatrix", r.modelMatrix);
    }
    else
    {
        graphics.SetUniform("modelMatrix", r.modelMatrix);
        graphics.SetUniform("normalMatrix", r.normalMatrix);

        graphics.SetLight(*light);

        if (r.material)
            graphics.BindMaterial(*r.material);
    }

    if (r.boneMatrices && !r.boneMatrices->empty())
    {
        for (size_t i = 0; i < r.boneMatrices->size(); i++)
        {
            graphics.SetUniform("bones[" + std::to_string(i) + "]", (*r.boneMatrices)[i]);
        }
    }

    graphics.BindResource(ResourceType::VERTEX_BUFFER, *r.vao);

    if (r.indexCount > 0)
        graphics.DrawIndexed(r.indexCount);
    else
        graphics.DrawNonIndexed(r.vertexCount);
}

void Renderer::UpdateCameraUBO()
{
    if (!camera)
        return;

    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection = camera->GetProjectionMatrix();

    CameraUBO data;
    data.view = view;
    data.projection = projection;
    data.viewProjection = projection * view;
    data.position = glm::vec4(camera->position, 1.0f);

    cameraUBO.SetData(&data, sizeof(data));
}

void Renderer::UpdateLightingUBO()
{
    if (!light)
        return;

    LightingUBO data;
    data.direction = glm::vec4(light->direction, 0.0f);
    data.ambient = glm::vec4(light->color * 0.1f, 1.0f);
    data.diffuse = glm::vec4(light->color * light->intensity, 1.0f);
    data.specular = glm::vec4(light->color * light->intensity, 1.0f);
    data.lightSpaceMatrix = light->lightSpaceMatrix;
    data.shadowParams = glm::vec4(
        shadowsEnabled ? 1.0f : 0.0f,
        0.005f,  // bias - you could add this to DirectionalLight if you want it configurable
        0.0f,
        0.0f
    );

    lightingUBO.SetData(&data, sizeof(data));
}