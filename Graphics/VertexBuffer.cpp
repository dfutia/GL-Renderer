#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "VertexBuffer.h"
#include "Platform/File.h"
#include "Rendering/Mesh.h"

VertexBuffer::VertexBuffer()
{
	glGenBuffers(1, &id);
}

VertexBuffer::VertexBuffer(const VertexBuffer& other)
{
	id = other.id;
}

VertexBuffer::VertexBuffer(const void* data, size_t length, BufferUsage usage)
{
	glGenBuffers(1, &id);
	Data(data, length, usage);
}

VertexBuffer::VertexBuffer(const Mesh& mesh, BufferUsage usage, std::function<void(const Vertex& v, VertexDataBuffer& data)> f)
{
	VertexDataBuffer data;
	const Vertex* vertices = mesh.Vertices();
	unsigned int vertexCount = mesh.VertexCount();

	for (unsigned int i = 0; i < vertexCount; i++)
	{
		f(vertices[i], data);
	}

	glGenBuffers(1, &id);
	Data(data.Pointer(), data.Size(), usage);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &id);
}

const VertexBuffer& VertexBuffer::operator=(const VertexBuffer& other)
{
	if (this != &other)
	{
		id = other.id;
	}
	return *this;
}

void VertexBuffer::Data(const void* data, size_t length, BufferUsage usage)
{
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, length, data, usage);
}

void VertexBuffer::SubData(const void* data, size_t offset, size_t length)
{
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferSubData(GL_ARRAY_BUFFER, offset, length, data);
}

void VertexBuffer::GetSubData(void* data, size_t offset, size_t length)
{
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glGetBufferSubData(GL_ARRAY_BUFFER, offset, length, data);
}