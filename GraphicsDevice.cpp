#include "PCH.h"
#include "GraphicsDevice.h"

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