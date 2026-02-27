#ifndef SKYBOX_H
#define SKYBOX_H

#include <memory>
#include "Graphics/VertexArray.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Texture.h"

struct Skybox
{
	std::shared_ptr<VertexArray> vao;
	std::shared_ptr<VertexBuffer> vbo;
	ShaderProgram shader;
	std::shared_ptr<Texture> cubemap;
};

Skybox LoadSkybox(const std::array<std::string, 6>& faces);
Skybox LoadSkybox(const std::filesystem::path& folder);

#endif