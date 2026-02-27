#ifndef GRAPHICS_DEVICE_H
#define GRAPHICS_DEVICE_H

class VertexArray;
class VertexBuffer;
class ShaderProgram;
class FrameBuffer;
class Texture;

enum class DepthFunc { Less, LessEqual, Equal, Always };

// GPU operations
class GraphicsDevice
{
public:
	enum BarrierFlags
	{
		ShaderStorageBarrier = GL_SHADER_STORAGE_BARRIER_BIT,
		VertexAttribBarrier = GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT,
		BufferUpdateBarrier = GL_BUFFER_UPDATE_BARRIER_BIT,
		TextureFetchBarrier = GL_TEXTURE_FETCH_BARRIER_BIT,
		ImageAccessBarrier = GL_SHADER_IMAGE_ACCESS_BARRIER_BIT,
		AllBarriers = GL_ALL_BARRIER_BITS,
	};

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
	void SetCullFaceMode(bool front);
	void SetDepthWrite(bool enabled);
	void SetDepthFunc(DepthFunc func);

	// Resource binding
	void BindShader(const ShaderProgram& shader);
	void BindVertexBuffer(const VertexBuffer& vbo);
	void BindIndexBuffer(const VertexBuffer& ibo);
	void BindVertexArray(const VertexArray& vao);
	void BindTexture(const Texture& texture, int slot = 0);
	void BindCubemap(const Texture& texture, int slot = 0);
	void BindFramebuffer(const FrameBuffer* fbo = nullptr);

	// Drawing
	void DrawIndexed(unsigned int numTriangle);
	void DrawInstanced(unsigned int numTriangle, unsigned int numInstance);
	void DrawNonIndexed(unsigned int numVertices); // i.e glDrawArrays

	void DispatchCompute(unsigned int groupsX, unsigned int groupsY = 1, unsigned int groupsZ = 1);
	void MemoryBarrier(BarrierFlags flags);
private:
	float clearR = 0.0f, clearG = 0.0f, clearB = 0.0f, clearA = 1.0f;
};

#endif 