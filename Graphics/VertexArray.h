#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

class VertexBuffer;
class ShaderProgram;

class VertexArray
{
public:
	typedef unsigned int Attribute;

	VertexArray();
	~VertexArray();

	// Delete copy operations - use shared_ptr instead
	VertexArray(const VertexArray&) = delete;
	VertexArray& operator=(const VertexArray&) = delete;

    // Allow move operations
    VertexArray(VertexArray&& other) noexcept : id(other.id) { other.id = 0; }
    VertexArray& operator=(VertexArray&& other) noexcept {
        if (this != &other) {
            glDeleteVertexArrays(1, &id);
            id = other.id;
            other.id = 0;
        }
        return *this;
    }

	operator unsigned int() const { return id; }
	//const VertexArray& operator=(const VertexArray& other);

	void BindAttribute(const Attribute& attribute, const VertexBuffer& buffer, GLenum type, unsigned int count, unsigned int stride, intptr_t offset);
	void BindIntAttribute(const Attribute& attribute, const VertexBuffer& buffer, GLenum type, unsigned int count, unsigned int stride, intptr_t offset);
	void BindElemenets(const VertexBuffer& elements);
	void BindTransformFeedback(unsigned int index, const VertexBuffer& buffer);
private:
	unsigned int id;
};

#endif