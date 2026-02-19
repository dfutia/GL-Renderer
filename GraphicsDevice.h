#ifndef GRAPHICS_DEVICE_H
#define GRAPHICS_DEVICE_H

enum class ResourceType
{
	SHADER_PROGRAM,
	VERTEX_BUFFER,
	INDEX_BUFFER
};

class GraphicsDevice
{
public:
	GraphicsDevice();
	~GraphicsDevice();

	// Viewport
	void SetViewport(int x, int y, int width, int height);

	// Clearing
	void SetClearColor(float r, float g, float b, float a = 1.0f);
	void Clear(bool color = true, bool depth = true, bool stencil = false);

	// Render state
	void SetDepthTest(bool enabled);
	void SetBlending(bool enabled);
	void SetCullFace(bool enabled);

	void BindResource(ResourceType type, unsigned int id);
	void BindFrameBuffer(int fbo = 0);

	void DrawIndexed(unsigned int numTriangle);
	void DrawInstanced(unsigned int numTriangle, unsigned int numInstance);
	void DrawNonIndexed(unsigned int numVertices);
private:
	float clearR = 0.0f, clearG = 0.0f, clearB = 0.0f, clearA = 1.0f;
};

#endif 