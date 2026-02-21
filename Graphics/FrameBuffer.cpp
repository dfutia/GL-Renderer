#include "PCH.h"
#include "Framebuffer.h"
#include "Texture.h"

#define PUSHSTATE() \
	GLint prevFBO = 0; \
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);

#define POPSTATE() \
	glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);

FrameBuffer::FrameBuffer(const FrameBuffer& other)
	: id(other.id), width(other.width), height(other.height),
	textureColor(other.textureColor), depthTexture(other.depthTexture)
{
}

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height, Type type, unsigned char colorBits, unsigned char depthBits)
	: width(width), height(height), textureColor(nullptr), depthTexture(nullptr)
{
	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	// Create color texture
	if (type == ColorAndDepth || type == ColorOnly)
	{
		textureColor = new Texture();
		GLenum colorInternalFormat;
		GLenum colorFormat = GL_RGBA;
		GLenum colorType = GL_UNSIGNED_BYTE;

		if (colorBits == 32)
			colorInternalFormat = GL_RGBA8;
		else if (colorBits == 16)
			colorInternalFormat = GL_RGBA16;
		else
			throw std::runtime_error("Unsupported color format for FrameBuffer");

		textureColor->Image2D(nullptr, colorType, colorFormat, width, height, colorInternalFormat);
		textureColor->SetWrapping(Texture::ClampTEdge);
		textureColor->SetFilters(Texture::Linear, Texture::Linear);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, (unsigned int)(*textureColor), 0);
	}

	// Create depth texture
	if (type == ColorAndDepth || type == DepthOnly)
	{
		depthTexture = new Texture();
		GLenum depthInternalFormat;
		GLenum depthFormat = GL_DEPTH_COMPONENT;
		GLenum depthType;

		if (depthBits == 24)
		{
			depthInternalFormat = GL_DEPTH_COMPONENT24;
			depthType = GL_UNSIGNED_INT;
		}
		else if (depthBits == 32)
		{
			depthInternalFormat = GL_DEPTH_COMPONENT32F;
			depthType = GL_FLOAT;
		}
		else
			throw std::runtime_error("Unsupported depth format for FrameBuffer");

		depthTexture->Image2D(nullptr, depthType, depthFormat, width, height, depthInternalFormat);
		depthTexture->SetWrapping(Texture::ClampToBorder);
		depthTexture->SetFilters(Texture::Nearest, Texture::Nearest);
		depthTexture->SetBorderColor();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, (unsigned int)(*depthTexture), 0);
	}

	// For depth-only, disable color buffer
	if (type == DepthOnly)
	{
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::runtime_error("Failed to create FrameBuffer");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &id);
	delete textureColor;
	delete depthTexture;
}

const FrameBuffer& FrameBuffer::operator=(const FrameBuffer& other)
{
	if (this != &other)
	{
		id = other.id;
		width = other.width;
		height = other.height;
		textureColor = other.textureColor;
		depthTexture = other.depthTexture;
	}
	return *this;
}

const Texture& FrameBuffer::GetColorTexture() const
{
	return *textureColor;
}

const Texture& FrameBuffer::GetDepthTexture() const
{
	return *depthTexture;
}