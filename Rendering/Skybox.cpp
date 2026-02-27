#include "PCH.h"
#include "Skybox.h"
#include "Platform/File.h"
#include "Graphics/Shader.h"

static float skyboxVertices[] = {
	-1,  1, -1,  -1, -1, -1,   1, -1, -1,   1, -1, -1,   1,  1, -1,  -1,  1, -1,
	-1, -1,  1,  -1, -1, -1,  -1,  1, -1,  -1,  1, -1,  -1,  1,  1,  -1, -1,  1,
	 1, -1, -1,   1, -1,  1,   1,  1,  1,   1,  1,  1,   1,  1, -1,   1, -1, -1,
	-1, -1,  1,  -1,  1,  1,   1,  1,  1,   1,  1,  1,   1, -1,  1,  -1, -1,  1,
	-1,  1, -1,   1,  1, -1,   1,  1,  1,   1,  1,  1,  -1,  1,  1,  -1,  1, -1,
	-1, -1, -1,  -1, -1,  1,   1, -1, -1,   1, -1, -1,  -1, -1,  1,   1, -1,  1
};

Skybox LoadSkybox(const std::array<std::string, 6>& faces)
{
	Skybox skybox;

	skybox.vbo = std::make_shared<VertexBuffer>(skyboxVertices, sizeof(skyboxVertices), VertexBuffer::StaticDraw);
	skybox.vao = std::make_shared<VertexArray>();
	skybox.vao->BindAttribute(0, *skybox.vbo, GL_FLOAT, 3, sizeof(float) * 3, 0);

	skybox.shader = ShaderProgram(
		Shader(Shader::Vertex, ReadTextFile(GetMediaPath() / "Shaders/skybox.vert")),
		Shader(Shader::Fragment, ReadTextFile(GetMediaPath() / "Shaders/skybox.frag"))
	);

	skybox.cubemap = LoadCubemap(faces);

	return skybox;
}

Skybox LoadSkybox(const std::filesystem::path& folder)
{
	return LoadSkybox({
		(folder / "right.jpg").string(),
		(folder / "left.jpg").string(),
		(folder / "top.jpg").string(),
		(folder / "bottom.jpg").string(),
		(folder / "front.jpg").string(),
		(folder / "back.jpg").string()
		});
}