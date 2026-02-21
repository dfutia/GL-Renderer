#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

class Texture;

class FramebufferException
{

};

class FrameBuffer
{
public:
	enum Type
	{
		ColorAndDepth,
		ColorOnly,
		DepthOnly
	};

	FrameBuffer(const FrameBuffer& other);
	FrameBuffer(unsigned int width, unsigned int height, Type type = ColorAndDepth, unsigned char colorBits = 32, unsigned char depthBits = 24);
	~FrameBuffer();

	operator unsigned int() const { return id; }
	const FrameBuffer& operator=(const FrameBuffer& other);

	bool HasColorTexture() const { return textureColor != nullptr; }
	bool HasDepthTexture() const { return depthTexture != nullptr; }

	const Texture& GetColorTexture() const;
	const Texture& GetDepthTexture() const;

	unsigned int GetWidth() const { return width; }
	unsigned int GetHeight() const { return height; }
private:
	unsigned int id;
	unsigned int width;
	unsigned int height;
	Texture* textureColor;
	Texture* depthTexture;
};

#endif