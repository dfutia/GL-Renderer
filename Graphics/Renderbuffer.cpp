#include "PCH.h"
#include "Renderbuffer.h"

Renderbuffer::Renderbuffer()
{
    glGenRenderbuffers(1, &id);
}

Renderbuffer::~Renderbuffer()
{
    if (id)
        glDeleteRenderbuffers(1, &id);
}

Renderbuffer::Renderbuffer(Renderbuffer&& other) noexcept
    : id(other.id)
{
    other.id = 0;
}

Renderbuffer& Renderbuffer::operator=(Renderbuffer&& other) noexcept
{
    if (this != &other)
    {
        if (id)
            glDeleteRenderbuffers(1, &id);

        id = other.id;
        other.id = 0;
    }
    return *this;
}

void Renderbuffer::Storage(GLenum internalFormat, unsigned int width, unsigned int height)
{
    glBindRenderbuffer(GL_RENDERBUFFER, id);
    glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
}

void Renderbuffer::StorageMultisample(unsigned int samples, GLenum internalFormat,
    unsigned int width, unsigned int height)
{
    glBindRenderbuffer(GL_RENDERBUFFER, id);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internalFormat, width, height);
}