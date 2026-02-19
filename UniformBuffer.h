#ifndef UNIFORM_BUFFER_H
#define UNIFORM_BUFFER_H

class UniformBuffer
{
public:
	class UniformBuffer();
	class UniformBuffer(const UniformBuffer& other);
	//class UniformBuffer();

	~UniformBuffer();

	const UniformBuffer& operator=(const UniformBuffer& other);

	void Data();
	void SubData();
	void GetSubData();
	void BindToIndex();
	void BindRange();
	static unsigned int GetUniformBlockIndex();
	static void BindUniformBlock();
	static size_t GetUniformBlockSize();
private:
	unsigned int id;
};

// Helper class for building uniform buffer data with proper alignment following std140 layout rules
class UniformBufferBuilder
{
public:
	UniformBufferBuilder() : currentOffset(0) {}

	void Add(float value);
	void Add(int value);
	void Add(unsigned int value);
	void Add(bool value);
	void Add(const glm::vec2& value);
	void Add(const glm::vec3& value);
	void Add(const glm::vec4& value);
	void Add(const glm::mat3& value);
	void Add(const glm::mat4& value);
	void Add(const float* values, unsigned int count);
	void Add(const glm::vec3* values, unsigned int count);
	void Add(const glm::vec4* values, unsigned int count);

	void AddPadding(size_t bytes);
	void AlignTo(size_t alignment);

	const void* GetData() const {}
	size_t GetSize() {}

	void Clear() {}

	UniformBuffer CreateBuffer() {}
private:
	std::vector<unsigned int> data;
	size_t currentOffset;

	void EnsureAlignment();
	void AddBytes();
};


#endif 