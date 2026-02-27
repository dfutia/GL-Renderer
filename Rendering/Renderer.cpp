#include "PCH.h"
#include "Renderer.h"

#include "Camera.h"
#include "Light.h"
#include "Material.h"
#include "Skybox.h"

Renderer::Renderer(GraphicsDevice& device, ShaderLibrary& shaders,
    unsigned int width, unsigned int height, unsigned int msaaSamples)
    : device(device)
    , shaders(shaders)
    , viewportWidth(width)
    , viewportHeight(height)
    , msaaSamples(msaaSamples)
    , msaaFBO(width, height, FrameBuffer::ColorAndDepth, 32, 24, msaaSamples)
    , resolveFBO(width, height, FrameBuffer::ColorOnly, 32, 0, 1)
    , cameraUBO(sizeof(CameraUBO))
    , lightingUBO(sizeof(LightingUBO))
    , screenQuad(CreateScreenQuad())
{
    cameraUBO.Bind(0);
    lightingUBO.Bind(1);
}

void Renderer::SetViewport(unsigned int width, unsigned int height)
{
    viewportWidth = width;
    viewportHeight = height;

    msaaFBO = FrameBuffer(width, height, FrameBuffer::ColorAndDepth, 32, 24, msaaSamples);
    resolveFBO = FrameBuffer(width, height, FrameBuffer::ColorOnly, 32, 0, 1);
}

void Renderer::Render(const RenderBatch& batch)
{
    if (!camera || !light)
        return;

    resolved = false;

    if (shadowsEnabled && !shadowMap)
    {
        shadowMap = std::make_unique<FrameBuffer>(
            light->shadowWidth,
            light->shadowHeight,
            FrameBuffer::DepthOnly, 0, 24
        );
    }

    if (shadowsEnabled)
        ShadowPass(batch);

    UpdateCameraUBO();
    UpdateLightingUBO();

    device.BindFramebuffer(nullptr);
    device.SetViewport(0, 0, viewportWidth, viewportHeight);
    device.SetClearColor(0.0f, 0.0f, 0.0f);
    device.Clear(true, true, false);

    ScenePass(batch);

    if (skybox)
        SkyboxPass();
}

const Texture& Renderer::GetResult()
{
    if (msaaSamples > 1 && !resolved)
    {
        msaaFBO.Resolve(resolveFBO);
        resolved = true;
    }

    return msaaSamples > 1
        ? resolveFBO.GetColorTexture()
        : msaaFBO.GetColorTexture();
}

void Renderer::Blit(const Texture& source, ShaderProgram& shader)
{
    device.BindFramebuffer(nullptr);
    device.SetViewport(0, 0, viewportWidth, viewportHeight);
    DrawScreenQuad(screenQuad, shader, source);
}

void Renderer::Blit(const Texture& source, ShaderProgram& shader, FrameBuffer& target)
{
    device.BindFramebuffer(&target);
    device.SetViewport(0, 0, target.GetWidth(), target.GetHeight());
    DrawScreenQuad(screenQuad, shader, source);
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

    device.SetViewport(0, 0, light->shadowWidth, light->shadowHeight);
    device.BindFramebuffer(shadowMap.get());
    device.Clear(false, true, false);
    device.SetDepthTest(true);

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
    device.SetDepthTest(true);

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
                device.SetBlending(true);
                device.SetDepthWrite(false);
            }
            else
            {
                device.SetBlending(false);
                device.SetDepthWrite(true);
            }
        }

        DrawRenderable(r, view, projection, false);
    }

    device.SetBlending(false);
    device.SetDepthWrite(true);
}

void Renderer::SkyboxPass()
{
    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection = camera->GetProjectionMatrix();
    DrawSkybox(*skybox, view, projection);
}

void Renderer::DrawRenderable(const Renderable& r, const glm::mat4& view,
    const glm::mat4& projection, bool shadowPass)
{
    std::string shaderName = shadowPass ? r.shadowShader : r.shader;
    ShaderProgram* shader = shaders.Get(shaderName);
    if (!shader)
        return;

    device.BindShader(*shader);

    if (shadowPass)
    {
        shader->SetUniform("lightSpaceMatrix", light->lightSpaceMatrix);
        shader->SetUniform("modelMatrix", r.modelMatrix);
    }
    else
    {
        shader->SetUniform("modelMatrix", r.modelMatrix);
        shader->SetUniform("normalMatrix", r.normalMatrix);

        ApplyLight(*shader, *light);

        if (r.material)
            ApplyMaterial(*shader, *r.material);
    }

    if (r.boneMatrices && !r.boneMatrices->empty())
    {
        for (size_t i = 0; i < r.boneMatrices->size(); i++)
        {
            shader->SetUniform("bones[" + std::to_string(i) + "]", (*r.boneMatrices)[i]);
        }
    }

    device.BindVertexArray(*r.vao);

    if (r.indexCount > 0)
        device.DrawIndexed(r.indexCount);
    else
        device.DrawNonIndexed(r.vertexCount);
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
        0.005f,
        0.0f,
        0.0f
    );

    lightingUBO.SetData(&data, sizeof(data));
}

void Renderer::DrawSkybox(const Skybox& skybox, const glm::mat4& view, const glm::mat4& projection)
{
    device.SetDepthWrite(false);
    device.SetDepthFunc(DepthFunc::LessEqual);

    device.BindShader(skybox.shader);
    skybox.shader.SetUniform("viewMatrix", glm::mat4(glm::mat3(view)));
    skybox.shader.SetUniform("projectionMatrix", projection);
    skybox.shader.SetUniform("skybox", 0);

    device.BindCubemap(*skybox.cubemap, 0);  // Dereference the shared_ptr
    device.BindVertexArray(*skybox.vao);
    device.DrawNonIndexed(36);

    device.SetDepthWrite(true);
    device.SetDepthFunc(DepthFunc::Less);
}
void Renderer::DrawScreenQuad(const ScreenQuad& quad, const ShaderProgram& shader, const Texture& texture)
{
    device.SetDepthTest(false);

    device.BindShader(shader);
    shader.SetUniform("uScreenTexture", 0);

    device.BindTexture(texture, 0);
    device.BindVertexArray(quad.vao);
    device.DrawNonIndexed(6);

    device.SetDepthTest(true);
}

void Renderer::ApplyMaterial(const ShaderProgram& shader, const Material& material)
{
    int slot = 0;
    for (const auto& [name, texture] : material.GetAllTextures())
    {
        device.BindTexture(*texture, slot);  // Dereference the shared_ptr
        shader.SetUniform(name, slot);
        slot++;
    }

    shader.SetUniform("hasDiffuseTexture", material.HasTexture(Material::DIFFUSE) ? 1 : 0);
    shader.SetUniform("hasNormalMap", material.HasTexture(Material::NORMAL) ? 1 : 0);
    shader.SetUniform("hasHeightMap", material.HasTexture(Material::HEIGHT) ? 1 : 0);

    shader.SetUniform("material.ambient", material.properties.ambient);
    shader.SetUniform("material.diffuse", material.properties.diffuse);
    shader.SetUniform("material.specular", material.properties.specular);
    shader.SetUniform("material.shininess", material.properties.shininess);
    shader.SetUniform("material.alpha", material.properties.alpha);
}

void Renderer::ApplyLight(const ShaderProgram& shader, const DirectionalLight& light)
{
    shader.SetUniform("light.direction", light.direction);
    shader.SetUniform("light.ambient", light.color * 0.1f);
    shader.SetUniform("light.diffuse", light.color * light.intensity);
    shader.SetUniform("light.specular", light.color * light.intensity);

    if (light.shadowMap)
    {
		device.BindTexture(*light.shadowMap, 10);
        shader.SetUniform("shadowMap", 10);
        shader.SetUniform("hasShadowMap", 1);
    }
    else
    {
        shader.SetUniform("hasShadowMap", 0);
    }
}