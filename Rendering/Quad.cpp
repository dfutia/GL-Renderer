#include "PCH.h"
#include "Quad.h"

static float quadVertices[] = {
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};

ScreenQuad CreateScreenQuad()
{
    ScreenQuad quad;

    quad.vbo = VertexBuffer(quadVertices, sizeof(quadVertices), VertexBuffer::StaticDraw);

    unsigned int stride = sizeof(float) * 4;
    quad.vao.BindAttribute(0, quad.vbo, GL_FLOAT, 2, stride, 0);
    quad.vao.BindAttribute(1, quad.vbo, GL_FLOAT, 2, stride, sizeof(float) * 2);

    return quad;
}