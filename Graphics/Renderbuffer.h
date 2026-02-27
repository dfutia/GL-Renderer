#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

class Renderbuffer
{
public:
    Renderbuffer();
    ~Renderbuffer();

    Renderbuffer(const Renderbuffer&) = delete;
    Renderbuffer& operator=(const Renderbuffer&) = delete;

    Renderbuffer(Renderbuffer&& other) noexcept;
    Renderbuffer& operator=(Renderbuffer&& other) noexcept;

    operator unsigned int() const { return id; }

    void Storage(GLenum internalFormat, unsigned int width, unsigned int height);
    void StorageMultisample(unsigned int samples, GLenum internalFormat, unsigned int width, unsigned int height);
private:
    unsigned int id = 0;
};

#endif