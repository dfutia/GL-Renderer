#ifndef TEXTURE_H
#define TEXTURE_H

class Texture
{
public:
	enum Format
	{
		RGB = 3,
		RGBA = 4,
		Grayscale = 1,
		GrayscaleAlpha = 2
	};

	enum DataType
	{
		UnsignedByte = GL_UNSIGNED_BYTE,
		Byte = GL_BYTE,
		UnsignedShort = GL_UNSIGNED_SHORT,
		Short = GL_SHORT,
		UnsignedInt = GL_UNSIGNED_INT,
		Int = GL_INT,
		HalfFloat = GL_HALF_FLOAT,
		Float = GL_FLOAT
	};

	enum WrapMode
	{
		Repeat = GL_REPEAT,
		MirroredRepeat = GL_MIRRORED_REPEAT,
		ClampTEdge = GL_CLAMP_TO_EDGE,
		ClampToBorder = GL_CLAMP_TO_BORDER
	};

	enum FilterMode
	{
		Nearest = GL_NEAREST,
		Linear = GL_LINEAR,
		NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
		LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
		NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
		LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR
	};

	Texture();
	Texture(const Texture& other);
	//Texture(const Image& image, Format format = Format::RGBA);
	//Texture(int width, int height, Format format);
	//Texture(int width, int height, Format format, const unsigned char* data);

	~Texture();

	operator unsigned int() const { return id; }
	const Texture& operator=(const Texture& other);

	void Image2D(const void* data, GLenum type, GLenum format, unsigned int width, unsigned int height, GLenum internalFormat);

	void SetWrapping(WrapMode wrapS);
	void SetWrapping(WrapMode wrapS, WrapMode wrapT);
	void SetWrapping(WrapMode wrapS, WrapMode wrapT, WrapMode wrapR);

	void SetFilters(FilterMode minFilter, FilterMode magFilter);

	void SetBorderColor();
	void SetBorderColor(float r, float g, float b, float a);

	void GenerateMipmaps();
private:
	unsigned int id;

	// moved from image class
	int width = 0;
	int height = 0;
	int channels = 0;
};

Texture LoadTextureHighQuality(const std::string& filepath);
Texture LoadTexture(const std::string& filepath); // from file
Texture LoadTexture(const unsigned char* data, int byteLength); // from memory
Texture LoadCubemap(const std::array<std::string, 6>& faces);

#endif 