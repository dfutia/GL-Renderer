#include "PCH.h"
#include "Framebuffer.h"
#include "Texture.h"

#define PUSHSTATE() \
	GLint prevFBO = 0; \
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);

#define POPSTATE() \
	glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height, unsigned char color, unsigned char depth)
{
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	// Create color texture
	textureColor = new Texture();
	GLenum colorInternalFormat;
	GLenum colorFormat = GL_RGBA;
	GLenum colorType = GL_UNSIGNED_BYTE;
	if (color == 32)
		colorInternalFormat = GL_RGBA8;
	else if (color == 16)
		colorInternalFormat = GL_RGBA16;
	else
		throw std::runtime_error("Unsupported color format for FrameBuffer");
	textureColor->Image2D(nullptr, colorType, colorFormat, width, height, colorInternalFormat);
	textureColor->SetWrapping(Texture::ClampTEdge);
	textureColor->SetFilters(Texture::Linear, Texture::Linear);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, (unsigned int)(*textureColor), 0);
	// Create depth texture
	if (depth > 0)
	{
		depthTexture = new Texture();
		GLenum depthInternalFormat;
		GLenum depthFormat = GL_DEPTH_COMPONENT;
		GLenum depthType;
		if (depth == 24)
		{
			depthInternalFormat = GL_DEPTH_COMPONENT24;
			depthType = GL_UNSIGNED_INT;
		}
		else if (depth == 32)
		{
			depthInternalFormat = GL_DEPTH_COMPONENT32;
			depthType = GL_FLOAT;
		}
		else
			throw std::runtime_error("Unsupported depth format for FrameBuffer");
		depthTexture->Image2D(nullptr, depthType, depthFormat, width, height, depthInternalFormat);
		depthTexture->SetWrapping(Texture::ClampTEdge);
		depthTexture->SetFilters(Texture::Linear, Texture::Linear);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, (unsigned int)(*depthTexture), 0);
	}
	else
	{
		depthTexture = nullptr;
	}
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::runtime_error("Failed to create FrameBuffer");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &id);
	delete textureColor;
	if (depthTexture)
		delete depthTexture;
}

const Texture& FrameBuffer::GetTexture()
{
	return *textureColor;
}

const Texture& FrameBuffer::GetDepthTexture()
{
	return *depthTexture;
}