#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

class UniformBuffer
{
public:
    UniformBuffer();
    UniformBuffer(size_t size);
    ~UniformBuffer();

    operator unsigned int() const { return id; }

    void Allocate(size_t size);
    void SetData(const void* data, size_t size, size_t offset = 0);
    void Bind(unsigned int bindingPoint) const;
private:
    unsigned int id;
};

#endif