#include "PCH.h"
#include "Framebuffer.h"
#include "Texture.h"

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height, Type type,
	unsigned char colorBits, unsigned char depthBits, unsigned int samples)
	: width(width), height(height), samples(samples)
{
	// Clamp samples to valid range
	if (samples < 1) samples = 1;
	if (samples > 1)
	{
		GLint maxSamples;
		glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
		if (samples > (unsigned int)maxSamples)
			samples = maxSamples;
	}
	this->samples = samples;

	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	// Determine color format
	GLenum colorInternalFormat = GL_RGBA8;
	GLenum colorFormat = GL_RGBA;
	GLenum colorType = GL_UNSIGNED_BYTE;

	if (colorBits == 16)
		colorInternalFormat = GL_RGBA16;
	else if (colorBits != 32 && (type == ColorAndDepth || type == ColorOnly))
		throw std::runtime_error("Unsupported color format for FrameBuffer");

	// Determine depth format
	GLenum depthInternalFormat = GL_DEPTH_COMPONENT24;
	GLenum depthFormat = GL_DEPTH_COMPONENT;
	GLenum depthType = GL_UNSIGNED_INT;

	if (depthBits == 32)
	{
		depthInternalFormat = GL_DEPTH_COMPONENT32F;
		depthType = GL_FLOAT;
	}
	else if (depthBits != 24 && (type == ColorAndDepth || type == DepthOnly))
		throw std::runtime_error("Unsupported depth format for FrameBuffer");

	// Create color attachment
	if (type == ColorAndDepth || type == ColorOnly)
	{
		textureColor = std::make_unique<Texture>();

		if (samples > 1)
		{
			textureColor->Image2DMultisample(samples, colorInternalFormat, width, height);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_2D_MULTISAMPLE, (unsigned int)(*textureColor), 0);
		}
		else
		{
			textureColor->Image2D(nullptr, colorType, colorFormat, width, height, colorInternalFormat);
			textureColor->SetWrapping(Texture::ClampTEdge);
			textureColor->SetFilters(Texture::Linear, Texture::Linear);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_2D, (unsigned int)(*textureColor), 0);
		}
	}

	// Create depth attachment
	if (type == ColorAndDepth || type == DepthOnly)
	{
		depthTexture = std::make_unique<Texture>();

		if (samples > 1)
		{
			depthTexture->Image2DMultisample(samples, depthInternalFormat, width, height);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
				GL_TEXTURE_2D_MULTISAMPLE, (unsigned int)(*depthTexture), 0);
		}
		else
		{
			depthTexture->Image2D(nullptr, depthType, depthFormat, width, height, depthInternalFormat);
			depthTexture->SetWrapping(Texture::ClampToBorder);
			depthTexture->SetFilters(Texture::Nearest, Texture::Nearest);
			depthTexture->SetBorderColor();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
				GL_TEXTURE_2D, (unsigned int)(*depthTexture), 0);
		}
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
	if (id != 0)
		glDeleteFramebuffers(1, &id);
}

FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept
	: id(other.id)
	, width(other.width)
	, height(other.height)
	, samples(other.samples)
	, textureColor(std::move(other.textureColor))
	, depthTexture(std::move(other.depthTexture))
{
	other.id = 0;
}

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other) noexcept
{
	if (this != &other)
	{
		// Clean up existing resources
		if (id != 0)
			glDeleteFramebuffers(1, &id);

		// Move from other
		id = other.id;
		width = other.width;
		height = other.height;
		samples = other.samples;
		textureColor = std::move(other.textureColor);
		depthTexture = std::move(other.depthTexture);

		other.id = 0;
	}
	return *this;
}

const Texture& FrameBuffer::GetColorTexture() const
{
	if (!textureColor)
		throw std::runtime_error("FrameBuffer has no color texture");
	return *textureColor;
}

const Texture& FrameBuffer::GetDepthTexture() const
{
	if (!depthTexture)
		throw std::runtime_error("FrameBuffer has no depth texture");
	return *depthTexture;
}

void FrameBuffer::Resolve(FrameBuffer& target) const
{
	Resolve(target.id, target.width, target.height);
}

void FrameBuffer::Resolve(unsigned int targetFBO, unsigned int targetWidth, unsigned int targetHeight) const
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, id);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFBO);

	if (textureColor)
	{
		glBlitFramebuffer(
			0, 0, width, height,
			0, 0, targetWidth, targetHeight,
			GL_COLOR_BUFFER_BIT,
			GL_LINEAR
		);
	}

	if (depthTexture)
	{
		glBlitFramebuffer(
			0, 0, width, height,
			0, 0, targetWidth, targetHeight,
			GL_DEPTH_BUFFER_BIT,
			GL_NEAREST
		);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}