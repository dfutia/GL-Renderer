#ifndef SHADER_STORAGE_BUFFER_H
#define SHADER_STORAGE_BUFFER_H

#include <glad/glad.h>

class ShaderStorageBuffer
{
public:
    enum BufferUsage
    {
        StaticDraw = GL_STATIC_DRAW,
        DynamicDraw = GL_DYNAMIC_DRAW,
        StreamDraw = GL_STREAM_DRAW,
        DynamicCopy = GL_DYNAMIC_COPY,
    };

    ShaderStorageBuffer()
    {
        glGenBuffers(1, &id);
    }

    ShaderStorageBuffer(const void* data, size_t size, BufferUsage usage)
    {
        glGenBuffers(1, &id);
        Data(data, size, usage);
    }

    ~ShaderStorageBuffer()
    {
        glDeleteBuffers(1, &id);
    }

    operator unsigned int() const { return id; }

    void Data(const void* data, size_t size, BufferUsage usage)
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usage);
    }

    void SubData(const void* data, size_t offset, size_t size)
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
    }

    void GetData(void* data, size_t offset, size_t size)
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
    }

    void BindBase(unsigned int bindingPoint)
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, id);
    }

private:
    unsigned int id;
};

#endif