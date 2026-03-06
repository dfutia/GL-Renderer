#ifndef SKYBOX_H
#define SKYBOX_H

#include <memory>
#include "Graphics/VertexArray.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/ShaderProgram.h"
#include "Graphics/Texture.h"
#include "Mesh.h"

struct Skybox
{
    std::reference_wrapper<Mesh> mesh;
    std::reference_wrapper<ShaderProgram> shader;
    std::reference_wrapper<Texture> texture;

    Skybox(Mesh& m, ShaderProgram& s, Texture& t) : mesh(m), shader(s), texture(t) {}
};

#endif