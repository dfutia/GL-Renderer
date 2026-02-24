#ifndef GRAPHICS_DEVICE_H
#define GRAPHICS_DEVICE_H

class ShaderProgram;
class Material;
class Texture;

struct ScreenQuad;
struct Skybox;
struct DirectionalLight;

enum class ResourceType
{
	SHADER_PROGRAM,
	VERTEX_BUFFER,
	INDEX_BUFFER
};

enum class DepthFunc { Less, LessEqual, Equal, Always };

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
	void SetCullFaceMode(bool front);
	void SetDepthWrite(bool enabled);
	void SetDepthFunc(DepthFunc func);

	void SetLight(const DirectionalLight& light);

	// Shader
	void BindShader(const ShaderProgram& shader);
	void SetUniform(const std::string& name, int value);
	void SetUniform(const std::string& name, float value);
	void SetUniform(const std::string& name, const glm::vec2& value);
	void SetUniform(const std::string& name, const glm::vec3& value);
	void SetUniform(const std::string& name, const glm::vec4& value);
	void SetUniform(const std::string& name, const glm::mat3& value);
	void SetUniform(const std::string& name, const glm::mat4& value);
	void SetUniform(const std::string& name, const std::vector<glm::mat4>& matrices);

	// Resource binding
	void BindMaterial(const Material& material);
	void BindResource(ResourceType type, unsigned int id);
	void BindCubemap(unsigned int id, int slot = 0);
	void BindFrameBuffer(int fbo = 0);

	// Drawing
	void DrawIndexed(unsigned int numTriangle);
	void DrawInstanced(unsigned int numTriangle, unsigned int numInstance);
	void DrawNonIndexed(unsigned int numVertices);

	void DrawSkybox(const Skybox& skybox, const glm::mat4& view, const glm::mat4& projection);
	void DrawScreenQuad(const ScreenQuad& quad, const ShaderProgram& shader, const Texture& texture);
private:
	float clearR = 0.0f, clearG = 0.0f, clearB = 0.0f, clearA = 1.0f;
	const ShaderProgram* currentShader = nullptr;
};

#endif 