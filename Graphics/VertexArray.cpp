#include "PCH.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "ShaderProgram.h"

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &id);
}

//VertexArray::VertexArray(const VertexArray& other)
//{
//	id = other.id;
//}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &id);
}

//const VertexArray& VertexArray::operator=(const VertexArray& other)
//{
//	if (this != &other)
//	{
//		id = other.id;
//	}
//	return *this;
//}

void VertexArray::BindAttribute(const Attribute& attribute, const VertexBuffer& buffer, GLenum type, unsigned int count, unsigned int stride, intptr_t offset)
{
	glBindVertexArray(id);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glEnableVertexAttribArray(attribute);
	glVertexAttribPointer(attribute, count, type, GL_FALSE, stride, reinterpret_cast<const void*>(offset));
}

void VertexArray::BindIntAttribute(const Attribute& attribute, const VertexBuffer& buffer, GLenum type, unsigned int count, unsigned int stride, intptr_t offset)
{
	glBindVertexArray(id);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glEnableVertexAttribArray(attribute);
	glVertexAttribIPointer(attribute, count, type, stride, reinterpret_cast<const void*>(offset));
}

void VertexArray::BindElemenets(const VertexBuffer& elements)
{
	glBindVertexArray(id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements);
}

void VertexArray::BindTransformFeedback(unsigned int index, const VertexBuffer& buffer)
{
	glBindVertexArray(id);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, index, buffer);
}