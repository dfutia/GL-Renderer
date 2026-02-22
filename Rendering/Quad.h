#ifndef SCREEN_QUAD_H
#define SCREEN_QUAD_H

#include "Graphics/VertexArray.h"
#include "Graphics/VertexBuffer.h"

struct ScreenQuad
{
	VertexArray vao;
	VertexBuffer vbo;
};

ScreenQuad CreateScreenQuad();

#endif