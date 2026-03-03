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

Skybox LoadSkybox(const std::array<std::filesystem::path, 6>& faces)
{
    Skybox skybox;

    skybox.vbo = std::make_shared<VertexBuffer>(skyboxVertices, sizeof(skyboxVertices), VertexBuffer::StaticDraw);
    skybox.vao = std::make_shared<VertexArray>();
    skybox.vao->BindAttribute(0, *skybox.vbo, GL_FLOAT, 3, sizeof(float) * 3, 0);

    skybox.shader = ShaderProgram(
        Shader(Shader::Vertex, ReadTextFile(GetMediaPath() / "Shaders/skybox.vert")),
        Shader(Shader::Fragment, ReadTextFile(GetMediaPath() / "Shaders/skybox.frag"))
    );

    auto result = LoadCubemap(faces);
    if (!result)
        throw std::runtime_error("Failed to load cubemap: " + result.error());

    skybox.cubemap = std::move(*result);

    return skybox;
}

Skybox LoadSkybox(const std::filesystem::path& folder)
{
    return LoadSkybox({
        folder / "right.jpg",
        folder / "left.jpg",
        folder / "top.jpg",
        folder / "bottom.jpg",
        folder / "front.jpg",
        folder / "back.jpg"
        });
}