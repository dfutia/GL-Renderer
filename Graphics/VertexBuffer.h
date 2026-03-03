#ifndef DATA_BUFFER_H
#define DATA_BUFFER_H

#include <utility>

struct Mesh;

static const int MAX_BONE_INFLUENCE = 4;

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	int boneIDs[4] = { -1, -1, -1, -1 }; 
	float boneWeights[4] = { 0, 0, 0, 0 }; 
	glm::vec3 tangent;
};

//struct SkinnedVertex
//{
//	glm::vec3 position;
//	glm::vec3 normal;
//	glm::vec2 texCoords;
//	int boneIDs[4];
//	float boneWeights[4];
//	glm::vec3 tangent;
//};

// Helper class for building vertex data
class VertexDataBuffer
{
public:
	void Float(float v) { Bytes((unsigned char*)&v, sizeof(v)); }
	void Int8(int8_t v) { Bytes((unsigned char*)&v, sizeof(v)); }
	void Int16(int16_t v) { Bytes((unsigned char*)&v, sizeof(v)); }
	void Int32(int32_t v) { Bytes((unsigned char*)&v, sizeof(v)); }
	void Uint8(uint8_t v) { Bytes((unsigned char*)&v, sizeof(v)); }
	void Uint16(uint16_t v) { Bytes((unsigned char*)&v, sizeof(v)); }
	void Uint32(uint32_t v) { Bytes((unsigned char*)&v, sizeof(v)); }

	void Vec2(const glm::vec2& v) { Bytes((unsigned char*)&v, sizeof(v)); }
	void Vec3(const glm::vec3& v) { Bytes((unsigned char*)&v, sizeof(v)); }
	void Vec4(const glm::vec4& v) { Bytes((unsigned char*)&v, sizeof(v)); }

	void* Pointer() { return &data[0]; }
	int Size() { return data.size(); }
private:
	std::vector<unsigned char> data;

	void Bytes(unsigned char* bytes, unsigned int count) {
		for (unsigned int i = 0; i < count; i++)
			data.push_back(bytes[i]);
	}
};

class VertexBuffer
{
public:
	enum BufferUsage
	{
		StreamDraw = GL_STREAM_DRAW,
		StreamRead = GL_STREAM_READ,
		StreamCopy = GL_STREAM_COPY,
		StaticDraw = GL_STATIC_DRAW,
		StaticRead = GL_STATIC_READ,
		StaticCopy = GL_STATIC_COPY,
		DynamicDraw = GL_DYNAMIC_DRAW,
		DynamicRead = GL_DYNAMIC_READ,
		DynamicCopy = GL_DYNAMIC_COPY,
	};

	VertexBuffer();
	VertexBuffer(const void* data, size_t length, BufferUsage usage);
	VertexBuffer(const Mesh& mesh, BufferUsage usage, std::function<void(const Vertex& v, VertexDataBuffer& data)> f);

	~VertexBuffer();

	// Delete copy operations - use shared_ptr instead
	VertexBuffer(const VertexBuffer&) = delete;
	VertexBuffer& operator=(const VertexBuffer&) = delete;

	// Allow move operations
	VertexBuffer(VertexBuffer&& other) noexcept : id(other.id) { other.id = 0; }
	VertexBuffer& operator=(VertexBuffer&& other) noexcept
	{
		if (this != &other)
		{
			if (id != 0)
				glDeleteBuffers(1, &id);
			id = other.id;
			other.id = 0;
		}
		return *this;
	}

	operator unsigned int() const { return id; }

	void Data(const void* data, size_t length, BufferUsage usage);
	void SubData(const void* data, size_t offset, size_t length);
	void GetSubData(void* data, size_t offset, size_t length);
private:
	unsigned int id = 0;
};


#endif 