#include <stb_image.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "PCH.h"
#include "Model.h"
#include "Platform/File.h"
#include "Mesh.h"
#include "Graphics/Texture.h"

static glm::mat4 ToGlm(const aiMatrix4x4& m)
{
    return glm::mat4(
        m.a1, m.b1, m.c1, m.d1,
        m.a2, m.b2, m.c2, m.d2,
        m.a3, m.b3, m.c3, m.d3,
        m.a4, m.b4, m.c4, m.d4
    );
}

static std::expected<Texture, std::string> LoadEmbeddedTexture(const aiTexture* aiTex)
{
    if (aiTex->mHeight == 0)
    {
        // Compressed (PNG/JPG) — mWidth is the byte count
        std::span<const std::uint8_t> data(reinterpret_cast<const std::uint8_t*>(aiTex->pcData), aiTex->mWidth);
        return LoadTexture(data);
    }
    else
    {
        // Uncompressed ARGB8888 — raw pixel data
        Texture texture;
        texture.Image2D(aiTex->pcData, GL_UNSIGNED_BYTE, GL_RGBA, aiTex->mWidth, aiTex->mHeight, GL_RGBA);
        texture.SetWrapping(Texture::Repeat, Texture::Repeat);
        texture.SetFilters(Texture::LinearMipmapLinear, Texture::Linear);
        texture.GenerateMipmaps();
        return texture;
    }
}

static std::optional<Texture> ExtractTexture(const aiScene* scene, const aiMesh* mesh, aiTextureType type)
{
    if (mesh->mMaterialIndex >= scene->mNumMaterials)
        return std::nullopt;

    aiMaterial* aiMat = scene->mMaterials[mesh->mMaterialIndex];

    aiString texPath;
    if (aiMat->GetTexture(type, 0, &texPath) != AI_SUCCESS)
        return std::nullopt;

    std::string path(texPath.C_Str());

    // Embedded texture referenced by index e.g. "*0"
    if (!path.empty() && path[0] == '*')
    {
        int texIndex = std::atoi(path.c_str() + 1);
        if (texIndex >= 0 && texIndex < (int)scene->mNumTextures)
        {
            auto result = LoadEmbeddedTexture(scene->mTextures[texIndex]);
            if (result)
                return std::move(*result);
        }
        return std::nullopt;
    }

    // Embedded texture referenced by filename
    if (const aiTexture* embedded = scene->GetEmbeddedTexture(path.c_str()))
    {
        auto result = LoadEmbeddedTexture(embedded);
        if (result)
            return std::move(*result);
        return std::nullopt;
    }

    // Match by filename only (strip path)
    std::string filename = path.substr(path.find_last_of("/\\") + 1);
    for (unsigned int i = 0; i < scene->mNumTextures; i++)
    {
        std::string embeddedName(scene->mTextures[i]->mFilename.C_Str());
        std::string embeddedFilename = embeddedName.substr(embeddedName.find_last_of("/\\") + 1);
        if (embeddedFilename == filename)
        {
            auto result = LoadEmbeddedTexture(scene->mTextures[i]);
            if (result)
                return std::move(*result);
            return std::nullopt;
        }
    }

    // Fall back to disk
    auto result = LoadTexture(std::filesystem::path(path));
    if (result)
        return std::move(*result);

    std::println("Warning: could not load texture '{}'", path);
    return std::nullopt;
}

static void BuildHierarchy(const aiNode* node, Skeleton& skeleton, int parentIndex)
{
    std::string rawName(node->mName.C_Str());
    std::string name = rawName;
    int boneIndex = skeleton.FindBone(name);

    int nextParent = parentIndex;

    if (boneIndex != -1)
    {
        skeleton.bones[boneIndex].parentIndex = parentIndex;
        skeleton.bones[boneIndex].localBindTransform = ToGlm(node->mTransformation);
        nextParent = boneIndex;
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        BuildHierarchy(node->mChildren[i], skeleton, nextParent);
    }
}

//Model LoadModel(const std::string& filepath)
//{
//    std::vector<std::uint8_t> fileData = ReadBinaryFile(filepath);
//
//    Assimp::Importer importer;
//
//    const aiScene* scene = importer.ReadFileFromMemory(
//        fileData.data(), fileData.size(),
//        aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace,
//        "fbx");
//
//    if (!scene || !scene->mNumMeshes)
//        throw std::runtime_error("Failed to load model: " + filepath);
//
//    aiMesh* aiM = scene->mMeshes[0];
//
//    // --- Read vertices ---
//    std::vector<Vertex> vertices;
//    vertices.reserve(aiM->mNumFaces * 3);
//
//    for (unsigned int i = 0; i < aiM->mNumFaces; i++)
//    {
//        aiFace& face = aiM->mFaces[i];
//        for (unsigned int j = 0; j < 3; j++)
//        {
//            unsigned int idx = face.mIndices[j];
//            Vertex vertex;
//            vertex.position = glm::vec3(aiM->mVertices[idx].x, aiM->mVertices[idx].y, aiM->mVertices[idx].z);
//
//            if (aiM->mNormals)
//                vertex.normal = glm::vec3(aiM->mNormals[idx].x, aiM->mNormals[idx].y, aiM->mNormals[idx].z);
//
//            if (aiM->mTextureCoords[0])
//                vertex.texCoords = glm::vec2(aiM->mTextureCoords[0][idx].x, aiM->mTextureCoords[0][idx].y);
//
//            if (aiM->mTangents)
//                vertex.tangent = glm::vec3(aiM->mTangents[idx].x, aiM->mTangents[idx].y, aiM->mTangents[idx].z);
//            else
//                vertex.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
//
//            vertices.push_back(vertex);
//        }
//    }
//
//    // --- Pack into VBO ---
//    VertexDataBuffer buffer;
//    for (const auto& v : vertices)
//    {
//        buffer.Vec3(v.position);
//        buffer.Vec3(v.normal);
//        buffer.Vec2(v.texCoords);
//        buffer.Vec3(v.tangent);
//    }
//
//    // --- Build VAO ---
//    Mesh mesh;
//    mesh.vertices = vertices;
//    mesh.vbo = std::make_shared<VertexBuffer>(buffer.Pointer(), buffer.Size(), VertexBuffer::StaticDraw);
//    mesh.vao = std::make_shared<VertexArray>();
//
//    unsigned int stride = sizeof(float) * 11;
//    mesh.vao->BindAttribute(0, *mesh.vbo, GL_FLOAT, 3, stride, 0);
//    mesh.vao->BindAttribute(1, *mesh.vbo, GL_FLOAT, 3, stride, sizeof(float) * 3);
//    mesh.vao->BindAttribute(2, *mesh.vbo, GL_FLOAT, 2, stride, sizeof(float) * 6);
//    mesh.vao->BindAttribute(3, *mesh.vbo, GL_FLOAT, 3, stride, sizeof(float) * 8);
//
//    // --- Build material ---
//    aiMaterial* aiMat = scene->mMaterials[aiM->mMaterialIndex];
//
//    Material material = Material::CreateDefault();
//
//    aiColor3D color;
//    float value;
//
//    if (aiMat->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
//        material.properties.ambient = glm::vec3(color.r, color.g, color.b);
//
//    if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
//        material.properties.diffuse = glm::vec3(color.r, color.g, color.b);
//
//    if (aiMat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
//        material.properties.specular = glm::vec3(color.r, color.g, color.b);
//
//    if (aiMat->Get(AI_MATKEY_SHININESS, value) == AI_SUCCESS)
//        material.properties.shininess = value;
//
//    if (aiMat->Get(AI_MATKEY_METALLIC_FACTOR, value) == AI_SUCCESS)
//        material.properties.metallic = value;
//
//    if (aiMat->Get(AI_MATKEY_ROUGHNESS_FACTOR, value) == AI_SUCCESS)
//        material.properties.roughness = value;
//
//    Model result;
//    result.mesh = std::move(mesh);
//    result.material = std::move(material);
//
//    // Extract and store textures in the model
//    if (auto tex = ExtractTexture(scene, aiM, aiTextureType_DIFFUSE))
//        result.textures.push_back(std::move(*tex));
//
//    if (auto tex = ExtractTexture(scene, aiM, aiTextureType_SPECULAR))
//        result.textures.push_back(std::move(*tex));
//
//    if (auto tex = ExtractTexture(scene, aiM, aiTextureType_NORMALS))
//        result.textures.push_back(std::move(*tex));
//
//    // Now set up material references (textures are stable in the vector)
//    size_t texIndex = 0;
//    if (ExtractTexture(scene, aiM, aiTextureType_DIFFUSE))
//        result.material.SetTexture(Material::DIFFUSE, result.textures[texIndex++]);
//
//    if (ExtractTexture(scene, aiM, aiTextureType_SPECULAR))
//        result.material.SetTexture(Material::SPECULAR, result.textures[texIndex++]);
//
//    if (ExtractTexture(scene, aiM, aiTextureType_NORMALS))
//        result.material.SetTexture(Material::NORMAL, result.textures[texIndex++]);
//
//    return result;
//}

std::vector<Animation> ExtractAnimations(const aiScene* scene)
{
    std::vector<Animation> animations;

    for (unsigned int i = 0; i < scene->mNumAnimations; i++)
    {
        aiAnimation* aiAnim = scene->mAnimations[i];

        Animation anim;
        anim.name = aiAnim->mName.C_Str();
        anim.duration = static_cast<float>(aiAnim->mDuration);
        anim.ticksPerSecond = aiAnim->mTicksPerSecond > 0
            ? static_cast<float>(aiAnim->mTicksPerSecond)
            : 24.0f;

        for (unsigned int j = 0; j < aiAnim->mNumChannels; j++)
        {
            aiNodeAnim* aiChannel = aiAnim->mChannels[j];
            std::string rawName = aiChannel->mNodeName.C_Str();
            std::string boneName = rawName;

            // Check if we already have a channel for this bone
            // (multiple AssimpFbx sub-nodes may map to the same bone)
            int existingIndex = -1;
            auto it = anim.boneNameToChannel.find(boneName);
            if (it != anim.boneNameToChannel.end())
                existingIndex = it->second;

            if (existingIndex != -1)
            {
                // Merge into existing channel
                BoneAnimation& channel = anim.channels[existingIndex];

                if (aiChannel->mNumPositionKeys > 1 ||
                    (aiChannel->mNumPositionKeys == 1 && channel.positionKeys.empty()))
                {
                    for (unsigned int k = 0; k < aiChannel->mNumPositionKeys; k++)
                    {
                        auto& key = aiChannel->mPositionKeys[k];
                        channel.positionKeys.push_back({
                            static_cast<float>(key.mTime),
                            glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z)
                            });
                    }
                }

                if (aiChannel->mNumRotationKeys > 1 ||
                    (aiChannel->mNumRotationKeys == 1 && channel.rotationKeys.empty()))
                {
                    for (unsigned int k = 0; k < aiChannel->mNumRotationKeys; k++)
                    {
                        auto& key = aiChannel->mRotationKeys[k];
                        channel.rotationKeys.push_back({
                            static_cast<float>(key.mTime),
                            glm::quat(key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z)
                            });
                    }
                }

                if (aiChannel->mNumScalingKeys > 1 ||
                    (aiChannel->mNumScalingKeys == 1 && channel.scaleKeys.empty()))
                {
                    for (unsigned int k = 0; k < aiChannel->mNumScalingKeys; k++)
                    {
                        auto& key = aiChannel->mScalingKeys[k];
                        channel.scaleKeys.push_back({
                            static_cast<float>(key.mTime),
                            glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z)
                            });
                    }
                }
            }
            else
            {
                // New channel
                BoneAnimation channel;
                channel.boneName = boneName;

                for (unsigned int k = 0; k < aiChannel->mNumPositionKeys; k++)
                {
                    auto& key = aiChannel->mPositionKeys[k];
                    channel.positionKeys.push_back({
                        static_cast<float>(key.mTime),
                        glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z)
                        });
                }

                for (unsigned int k = 0; k < aiChannel->mNumRotationKeys; k++)
                {
                    auto& key = aiChannel->mRotationKeys[k];
                    channel.rotationKeys.push_back({
                        static_cast<float>(key.mTime),
                        glm::quat(key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z)
                        });
                }

                for (unsigned int k = 0; k < aiChannel->mNumScalingKeys; k++)
                {
                    auto& key = aiChannel->mScalingKeys[k];
                    channel.scaleKeys.push_back({
                        static_cast<float>(key.mTime),
                        glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z)
                        });
                }

                anim.boneNameToChannel[boneName] = static_cast<int>(anim.channels.size());
                anim.channels.push_back(std::move(channel));
            }
        }

        animations.push_back(std::move(anim));
    }

    return animations;
}

Model LoadModel(const std::string& filepath)
{
    std::vector<std::uint8_t> fileData = ReadBinaryFile(filepath);

    Assimp::Importer importer;
    importer.SetPropertyInteger(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);

    const aiScene* scene = importer.ReadFileFromMemory(
        fileData.data(),
        fileData.size(),
        aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace,
        "fbx"
    );

    if (!scene || !scene->mNumMeshes)
        throw std::runtime_error("Failed to load model: " + filepath);

    aiMesh* aiM = scene->mMeshes[0];

    // Build skeleton from bone data
    Skeleton skeleton;
    skeleton.bones.resize(aiM->mNumBones);

    for (unsigned int i = 0; i < aiM->mNumBones; i++)
    {
        aiBone* bone = aiM->mBones[i];
        std::string name = bone->mName.C_Str();

        skeleton.bones[i].name = name;
        skeleton.bones[i].offsetMatrix = ToGlm(bone->mOffsetMatrix);
        skeleton.boneNameToIndex[name] = i;
    }

    BuildHierarchy(scene->mRootNode, skeleton, -1);

    // Read vertices
    std::vector<Vertex> vertices;
    vertices.resize(aiM->mNumVertices);

    for (unsigned int i = 0; i < aiM->mNumVertices; i++)
    {
        vertices[i].position = glm::vec3(aiM->mVertices[i].x, aiM->mVertices[i].y, aiM->mVertices[i].z);
        if (aiM->mNormals)
            vertices[i].normal = glm::vec3(aiM->mNormals[i].x, aiM->mNormals[i].y, aiM->mNormals[i].z);
        if (aiM->mTextureCoords[0])
            vertices[i].texCoords = glm::vec2(aiM->mTextureCoords[0][i].x, aiM->mTextureCoords[0][i].y);
        if (aiM->mTangents)
            vertices[i].tangent = glm::vec3(aiM->mTangents[i].x, aiM->mTangents[i].y, aiM->mTangents[i].z);
        for (int k = 0; k < 4; k++)
        {
            vertices[i].boneIDs[k] = -1;
            vertices[i].boneWeights[k] = 0.0f;
        }
    }

    // Assign bone weights to vertices
    for (unsigned int i = 0; i < aiM->mNumBones; i++)
    {
        aiBone* bone = aiM->mBones[i];
        for (unsigned int j = 0; j < bone->mNumWeights; j++)
        {
            unsigned int vertexId = bone->mWeights[j].mVertexId;
            float weight = bone->mWeights[j].mWeight;

            for (int k = 0; k < 4; k++)
            {
                if (vertices[vertexId].boneIDs[k] == -1)
                {
                    vertices[vertexId].boneIDs[k] = i;
                    vertices[vertexId].boneWeights[k] = weight;
                    break;
                }
            }
        }
    }

    // Build index buffer from faces
    std::vector<unsigned int> indices;
    indices.reserve(aiM->mNumFaces * 3);
    for (unsigned int i = 0; i < aiM->mNumFaces; i++)
    {
        aiFace& face = aiM->mFaces[i];
        for (unsigned int j = 0; j < 3; j++)
            indices.push_back(face.mIndices[j]);
    }

    // Pack into VBO
    VertexDataBuffer buffer;
    for (const auto& v : vertices)
    {
        buffer.Vec3(v.position);
        buffer.Vec3(v.normal);
        buffer.Vec2(v.texCoords);
        for (int j = 0; j < 4; j++) buffer.Int32(v.boneIDs[j]);
        for (int j = 0; j < 4; j++) buffer.Float(v.boneWeights[j]);
        buffer.Vec3(v.tangent);
    }

    // Build VAO
    Mesh mesh;
    mesh.vertices = vertices;
    mesh.indices = indices;
    mesh.skeleton = skeleton;
    mesh.vbo = std::make_shared<VertexBuffer>(buffer.Pointer(), buffer.Size(), VertexBuffer::StaticDraw);
    mesh.vao = std::make_shared<VertexArray>();

    unsigned int stride = sizeof(Vertex);
    mesh.vao->BindAttribute(0, *mesh.vbo, GL_FLOAT, 3, stride, offsetof(Vertex, position));
    mesh.vao->BindAttribute(1, *mesh.vbo, GL_FLOAT, 3, stride, offsetof(Vertex, normal));
    mesh.vao->BindAttribute(2, *mesh.vbo, GL_FLOAT, 2, stride, offsetof(Vertex, texCoords));
    mesh.vao->BindIntAttribute(3, *mesh.vbo, GL_INT, 4, stride, offsetof(Vertex, boneIDs));
    mesh.vao->BindAttribute(4, *mesh.vbo, GL_FLOAT, 4, stride, offsetof(Vertex, boneWeights));
    mesh.vao->BindAttribute(5, *mesh.vbo, GL_FLOAT, 3, stride, offsetof(Vertex, tangent));

    // Bind index buffer
    mesh.ebo = std::make_shared<VertexBuffer>();
    mesh.ebo->Data(indices.data(), indices.size() * sizeof(unsigned int), VertexBuffer::StaticDraw);
    mesh.vao->BindElemenets(*mesh.ebo);

    // Build material
    aiMaterial* aiMat = scene->mMaterials[aiM->mMaterialIndex];

    Material material = Material::CreateDefault();

    aiColor3D color;
    float value;

    if (aiMat->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
        material.properties.ambient = glm::vec3(color.r, color.g, color.b);

    if (aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
        material.properties.diffuse = glm::vec3(color.r, color.g, color.b);

    if (aiMat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
        material.properties.specular = glm::vec3(color.r, color.g, color.b);

    if (aiMat->Get(AI_MATKEY_SHININESS, value) == AI_SUCCESS)
        material.properties.shininess = value;

    if (aiMat->Get(AI_MATKEY_METALLIC_FACTOR, value) == AI_SUCCESS)
        material.properties.metallic = value;

    if (aiMat->Get(AI_MATKEY_ROUGHNESS_FACTOR, value) == AI_SUCCESS)
        material.properties.roughness = value;

    Model result;
    result.mesh = std::move(mesh);
    result.material = std::move(material);

    // Extract and store textures in the model
    if (auto tex = ExtractTexture(scene, aiM, aiTextureType_DIFFUSE))
        result.textures.push_back(std::move(*tex));

    if (auto tex = ExtractTexture(scene, aiM, aiTextureType_SPECULAR))
        result.textures.push_back(std::move(*tex));

    if (auto tex = ExtractTexture(scene, aiM, aiTextureType_NORMALS))
        result.textures.push_back(std::move(*tex));

    // Now set up material references (textures are stable in the vector)
    size_t texIndex = 0;
    if (ExtractTexture(scene, aiM, aiTextureType_DIFFUSE))
        result.material.SetTexture(Material::DIFFUSE, result.textures[texIndex++]);

    if (ExtractTexture(scene, aiM, aiTextureType_SPECULAR))
        result.material.SetTexture(Material::SPECULAR, result.textures[texIndex++]);

    if (ExtractTexture(scene, aiM, aiTextureType_NORMALS))
        result.material.SetTexture(Material::NORMAL, result.textures[texIndex++]);

    return result;
}

std::vector<Animation> LoadAnimations(const std::string& filepath)
{
    std::vector<std::uint8_t> fileData = ReadBinaryFile(filepath);

    Assimp::Importer importer;
    importer.SetPropertyInteger(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, 0);

    const aiScene* scene = importer.ReadFileFromMemory(
        fileData.data(), fileData.size(),
        aiProcess_Triangulate,
        "fbx");

    if (!scene)
        throw std::runtime_error("Failed to load animations: " + filepath);

    std::vector<Animation> animations;

    for (unsigned int i = 0; i < scene->mNumAnimations; i++)
    {
        aiAnimation* aiAnim = scene->mAnimations[i];

        Animation anim;
        anim.name = aiAnim->mName.C_Str();
        anim.duration = static_cast<float>(aiAnim->mDuration);
        anim.ticksPerSecond = aiAnim->mTicksPerSecond > 0
            ? static_cast<float>(aiAnim->mTicksPerSecond)
            : 24.0f;

        for (unsigned int j = 0; j < aiAnim->mNumChannels; j++)
        {
            aiNodeAnim* aiChannel = aiAnim->mChannels[j];
            std::string boneName = aiChannel->mNodeName.C_Str();

            int existingIndex = -1;
            auto it = anim.boneNameToChannel.find(boneName);
            if (it != anim.boneNameToChannel.end())
                existingIndex = it->second;

            if (existingIndex != -1)
            {
                BoneAnimation& channel = anim.channels[existingIndex];

                if (aiChannel->mNumPositionKeys > 1 ||
                    (aiChannel->mNumPositionKeys == 1 && channel.positionKeys.empty()))
                {
                    for (unsigned int k = 0; k < aiChannel->mNumPositionKeys; k++)
                    {
                        auto& key = aiChannel->mPositionKeys[k];
                        channel.positionKeys.push_back({
                            static_cast<float>(key.mTime),
                            glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z)
                            });
                    }
                }

                if (aiChannel->mNumRotationKeys > 1 ||
                    (aiChannel->mNumRotationKeys == 1 && channel.rotationKeys.empty()))
                {
                    for (unsigned int k = 0; k < aiChannel->mNumRotationKeys; k++)
                    {
                        auto& key = aiChannel->mRotationKeys[k];
                        channel.rotationKeys.push_back({
                            static_cast<float>(key.mTime),
                            glm::quat(key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z)
                            });
                    }
                }

                if (aiChannel->mNumScalingKeys > 1 ||
                    (aiChannel->mNumScalingKeys == 1 && channel.scaleKeys.empty()))
                {
                    for (unsigned int k = 0; k < aiChannel->mNumScalingKeys; k++)
                    {
                        auto& key = aiChannel->mScalingKeys[k];
                        channel.scaleKeys.push_back({
                            static_cast<float>(key.mTime),
                            glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z)
                            });
                    }
                }
            }
            else
            {
                BoneAnimation channel;
                channel.boneName = boneName;

                for (unsigned int k = 0; k < aiChannel->mNumPositionKeys; k++)
                {
                    auto& key = aiChannel->mPositionKeys[k];
                    channel.positionKeys.push_back({
                        static_cast<float>(key.mTime),
                        glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z)
                        });
                }

                for (unsigned int k = 0; k < aiChannel->mNumRotationKeys; k++)
                {
                    auto& key = aiChannel->mRotationKeys[k];
                    channel.rotationKeys.push_back({
                        static_cast<float>(key.mTime),
                        glm::quat(key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z)
                        });
                }

                for (unsigned int k = 0; k < aiChannel->mNumScalingKeys; k++)
                {
                    auto& key = aiChannel->mScalingKeys[k];
                    channel.scaleKeys.push_back({
                        static_cast<float>(key.mTime),
                        glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z)
                        });
                }

                anim.boneNameToChannel[boneName] = static_cast<int>(anim.channels.size());
                anim.channels.push_back(std::move(channel));
            }
        }

        animations.push_back(std::move(anim));
    }

    return animations;
}