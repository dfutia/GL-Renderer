#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <memory>

#include "Texture.h"

class Texture;

class FrameBuffer
{
public:
	enum Type
	{
		ColorAndDepth,
		ColorOnly,
		DepthOnly
	};

	FrameBuffer(unsigned int width, unsigned int height, Type type = ColorAndDepth,
		unsigned char colorBits = 32, unsigned char depthBits = 24, unsigned int samples = 1);
	~FrameBuffer();

	// Delete copy
	FrameBuffer(const FrameBuffer&) = delete;
	FrameBuffer& operator=(const FrameBuffer&) = delete;

	// Allow move
	FrameBuffer(FrameBuffer&& other) noexcept;
	FrameBuffer& operator=(FrameBuffer&& other) noexcept;

	operator unsigned int() const { return id; }

	bool HasColorTexture() const { return textureColor != nullptr; }
	bool HasDepthTexture() const { return depthTexture != nullptr; }

	const Texture& GetColorTexture() const;
	const Texture& GetDepthTexture() const;

	unsigned int GetWidth() const { return width; }
	unsigned int GetHeight() const { return height; }
	unsigned int GetSamples() const { return samples; }
	bool IsMultisampled() const { return samples > 1; }

	void Resolve(FrameBuffer& target) const;
	void Resolve(unsigned int targetFBO, unsigned int targetWidth, unsigned int targetHeight) const;

	std::string DebugInfo() const
	{
		std::ostringstream ss;
		ss << "FrameBuffer id=" << id << " " << width << "x" << height;
		if (samples > 1) ss << " MSAA x" << samples;
		ss << "\n";
		ss << "  Color: " << (textureColor ? textureColor->DebugInfo() : "none") << "\n";
		ss << "  Depth: " << (depthTexture ? depthTexture->DebugInfo() : "none") << "\n";
		return ss.str();
	}
private:
	unsigned int id = 0;
	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int samples = 1;

	std::unique_ptr<Texture> textureColor;
	std::unique_ptr<Texture> depthTexture;
};

#endif