#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

class VertexBuffer;
class ShaderProgram;

class VertexArray
{
public:
	typedef unsigned int Attribute;

	VertexArray();
	VertexArray(const VertexArray& other);

	~VertexArray();

	operator unsigned int() const { return id; }
	const VertexArray& operator=(const VertexArray& other);

	void BindAttribute(const Attribute& attribute, const VertexBuffer& buffer, GLenum type, unsigned int count, unsigned int stride, intptr_t offset);
	void BindIntAttribute(const Attribute& attribute, const VertexBuffer& buffer, GLenum type, unsigned int count, unsigned int stride, intptr_t offset);
	void BindElemenets(const VertexBuffer& elements);
	void BindTransformFeedback(unsigned int index, const VertexBuffer& buffer);
private:
	unsigned int id;
};

#endif