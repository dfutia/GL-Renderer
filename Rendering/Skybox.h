#ifndef SKYBOX_H
#define SKYBOX_H

#include "Graphics/VertexArray.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Texture.h"

struct Skybox
{
    VertexArray* vao = nullptr;
    VertexBuffer* vbo = nullptr;
    ShaderProgram shader;
    Texture cubemap;
};


Skybox LoadSkybox(const std::array<std::string, 6>& faces);
Skybox LoadSkybox(const std::filesystem::path& folder);

#endif 