#include "PCH.h"
#include "GraphicsDevice.h"
#include "ShaderProgram.h"
#include "Rendering/Material.h"
#include "Rendering/Skybox.h"
#include "Rendering/Light.h"

GraphicsDevice::GraphicsDevice()
{
}

GraphicsDevice::~GraphicsDevice()
{
}

void GraphicsDevice::SetViewport(int x, int y, int width, int height)
{
    glViewport(x, y, width, height);
}

void GraphicsDevice::SetClearColor(float r, float g, float b, float a)
{
    clearR = r; clearG = g; clearB = b; clearA = a;
    glClearColor(r, g, b, a);
}

void GraphicsDevice::Clear(bool color, bool depth, bool stencil)
{
    GLbitfield mask = 0;
    if (color)   mask |= GL_COLOR_BUFFER_BIT;
    if (depth)   mask |= GL_DEPTH_BUFFER_BIT;
    if (stencil) mask |= GL_STENCIL_BUFFER_BIT;
    glClear(mask);
}

void GraphicsDevice::SetDepthTest(bool enabled)
{
    if (enabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
}

void GraphicsDevice::SetBlending(bool enabled)
{
    if (enabled)
        glEnable(GL_BLEND);
    else
        glDisable(GL_BLEND);
}

void GraphicsDevice::SetCullFace(bool enabled)
{
    if (enabled)
        glEnable(GL_CULL_FACE);
    else
        glDisable(GL_CULL_FACE);
}

void GraphicsDevice::SetCullFaceMode(bool front)
{
    glCullFace(front ? GL_FRONT : GL_BACK);
}

void GraphicsDevice::SetDepthWrite(bool enabled)
{
    glDepthMask(enabled ? GL_TRUE : GL_FALSE);
}

void GraphicsDevice::SetDepthFunc(DepthFunc func)
{
    GLenum glFunc;
    switch (func)
    {
    case DepthFunc::Less:      glFunc = GL_LESS;   break;
    case DepthFunc::LessEqual: glFunc = GL_LEQUAL; break;
    case DepthFunc::Equal:     glFunc = GL_EQUAL;  break;
    case DepthFunc::Always:    glFunc = GL_ALWAYS; break;
    default:                   glFunc = GL_LESS;   break;
    }
    glDepthFunc(glFunc);
}

void GraphicsDevice::SetLight(const DirectionalLight& light)
{
    SetUniform("light.direction", light.direction);
    SetUniform("light.color", light.color);
    SetUniform("light.intensity", light.intensity);
    SetUniform("lightDirection", light.direction);
}


void GraphicsDevice::BindShader(const ShaderProgram& shader)
{
    currentShader = &shader;
    glUseProgram(shader);
}

void GraphicsDevice::SetUniform(const std::string& name, int value)
{
    if (currentShader)
        currentShader->SetUniform(currentShader->GetUniform(name), value);
}

void GraphicsDevice::SetUniform(const std::string& name, float value)
{
    if (currentShader)
        currentShader->SetUniform(currentShader->GetUniform(name), value);
}

void GraphicsDevice::SetUniform(const std::string& name, const glm::vec2& value)
{
    if (currentShader)
        currentShader->SetUniform(currentShader->GetUniform(name), value);
}

void GraphicsDevice::SetUniform(const std::string& name, const glm::vec3& value)
{
    if (currentShader)
        currentShader->SetUniform(currentShader->GetUniform(name), value);
}

void GraphicsDevice::SetUniform(const std::string& name, const glm::vec4& value)
{
    if (currentShader)
        currentShader->SetUniform(currentShader->GetUniform(name), value);
}

void GraphicsDevice::SetUniform(const std::string& name, const glm::mat3& value)
{
    if (currentShader)
        currentShader->SetUniform(currentShader->GetUniform(name), value);
}

void GraphicsDevice::SetUniform(const std::string& name, const glm::mat4& value)
{
    if (currentShader)
        currentShader->SetUniform(currentShader->GetUniform(name), value);
}

void GraphicsDevice::BindMaterial(const Material& material)
{
    if (!currentShader)
        return;

    // Bind textures by their slot
    for (const auto& [slot, texture] : material.textures)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture);
        SetUniform("texture" + std::to_string(slot), slot);
    }

    SetUniform("hasDiffuseTexture", material.HasTexture(Material::Diffuse) ? 1 : 0);
    SetUniform("hasNormalMap", material.HasTexture(Material::Normal) ? 1 : 0);

    // Bind properties
    SetUniform("material.ambient", material.properties.ambient);
    SetUniform("material.diffuse", material.properties.diffuse);
    SetUniform("material.specular", material.properties.specular);
    SetUniform("material.shininess", material.properties.shininess);
    SetUniform("material.alpha", material.properties.alpha);
}

void GraphicsDevice::BindResource(ResourceType type, unsigned int id)
{
    switch (type)
    {
    case ResourceType::SHADER_PROGRAM:
        glUseProgram(id);
        break;
    case ResourceType::VERTEX_BUFFER:
        glBindVertexArray(id);
        break;
    case ResourceType::INDEX_BUFFER:
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        break;
    }
}

void GraphicsDevice::BindCubemap(unsigned int id, int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
}

void GraphicsDevice::BindFrameBuffer(int fbo)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void GraphicsDevice::DrawIndexed(unsigned int numTriangle)
{
    glDrawElements(GL_TRIANGLES, numTriangle, GL_UNSIGNED_INT, 0);
}


void GraphicsDevice::DrawInstanced(unsigned int numTriangle, unsigned int numInstance)
{
    glDrawElementsInstanced(GL_TRIANGLES, numTriangle, GL_UNSIGNED_INT, 0, numInstance);
}

void GraphicsDevice::DrawNonIndexed(unsigned int numVertices)
{
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
}

void GraphicsDevice::DrawSkybox(const Skybox& skybox, const glm::mat4& view, const glm::mat4& projection)
{
    SetDepthWrite(false);
    SetDepthFunc(DepthFunc::LessEqual);

    BindShader(skybox.shader);

    glm::mat4 skyboxView = glm::mat4(glm::mat3(view));
    SetUniform("viewMatrix", skyboxView);
    SetUniform("projectionMatrix", projection);
    SetUniform("skybox", 0);

    BindCubemap(skybox.cubemap, 0);
    BindResource(ResourceType::VERTEX_BUFFER, *skybox.vao);
    DrawNonIndexed(36);

    SetDepthWrite(true);
    SetDepthFunc(DepthFunc::Less);
}