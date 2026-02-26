#include "PCH.h"
#include "UniformBuffer.h"

UniformBuffer::UniformBuffer()
{
    glGenBuffers(1, &id);
}

UniformBuffer::UniformBuffer(size_t size)
{
    glGenBuffers(1, &id);
    Allocate(size);
}

UniformBuffer::~UniformBuffer()
{
    glDeleteBuffers(1, &id);
}

void UniformBuffer::Allocate(size_t size)
{
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

void UniformBuffer::SetData(const void* data, size_t size, size_t offset)
{
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

void UniformBuffer::Bind(unsigned int bindingPoint) const
{
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, id);
}