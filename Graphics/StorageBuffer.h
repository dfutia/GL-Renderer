#ifndef STORAGE_BUFFER_H
#define STORAGE_BUFFER_H

class StorageBuffer
{
public:
    StorageBuffer()
    {
        glGenBuffers(1, &id);
    }

    ~StorageBuffer()
    {
        glDeleteBuffers(1, &id);
    }

    void Data(const void* data, size_t size, GLenum usage = GL_DYNAMIC_DRAW)
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
        glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usage);
    }

    void BindBase(unsigned int bindingPoint)
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, id);
    }

    void GetData(void* data, size_t offset, size_t size)
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
    }

    unsigned int GetID() const { return id; }
private:
    unsigned int id;
};

#endif