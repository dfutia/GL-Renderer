#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

class Texture;

class FramebufferException
{

};

class FrameBuffer
{
public:
	FrameBuffer(const FrameBuffer& other);
	FrameBuffer(unsigned int width, unsigned int height, unsigned char color = 32, unsigned char depth = 24);
	~FrameBuffer();

	operator unsigned int() const { return id; }
	const FrameBuffer& operator=(const FrameBuffer& other);

	const Texture& GetTexture();
	const Texture& GetDepthTexture();
private:
	unsigned int id;
	Texture* textureColor;
	Texture* depthTexture;
};

#endif