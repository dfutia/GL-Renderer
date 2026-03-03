#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>
#include <expected>
#include <string>
#include <filesystem>
#include <span>

class Texture
{
public:
	enum Format { RGB = 3, RGBA = 4, Grayscale = 1, GrayscaleAlpha = 2 };

	enum DataType
	{
		UnsignedByte = GL_UNSIGNED_BYTE, Byte = GL_BYTE,
		UnsignedShort = GL_UNSIGNED_SHORT, Short = GL_SHORT,
		UnsignedInt = GL_UNSIGNED_INT, Int = GL_INT,
		HalfFloat = GL_HALF_FLOAT, Float = GL_FLOAT
	};

	enum WrapMode
	{
		Repeat = GL_REPEAT, MirroredRepeat = GL_MIRRORED_REPEAT,
		ClampTEdge = GL_CLAMP_TO_EDGE, ClampToBorder = GL_CLAMP_TO_BORDER
	};

	enum FilterMode
	{
		Nearest = GL_NEAREST, Linear = GL_LINEAR,
		NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
		LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
		NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
		LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR
	};

	Texture();
	~Texture();

	// Delete copy
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	// Allow move
	Texture(Texture&& other) noexcept;
	Texture& operator=(Texture&& other) noexcept;

	operator unsigned int() const { return id; }

	void Image2D(const void* data, GLenum type, GLenum format, unsigned int width, unsigned int height, GLenum internalFormat);
	void Image2DMultisample(unsigned int samples, GLenum internalFormat, unsigned int width, unsigned int height);
	void SetWrapping(WrapMode wrapS);
	void SetWrapping(WrapMode wrapS, WrapMode wrapT);
	void SetWrapping(WrapMode wrapS, WrapMode wrapT, WrapMode wrapR);
	void SetFilters(FilterMode minFilter, FilterMode magFilter);
	void SetBorderColor();
	void SetBorderColor(float r, float g, float b, float a);
	void GenerateMipmaps();
	GLenum GetTarget() const { return target; }
private:
	unsigned int id = 0;
	GLenum target = GL_TEXTURE_2D;
};

std::expected<Texture, std::string> LoadTexture(const std::filesystem::path& filepath);
std::expected<Texture, std::string> LoadTexture(std::span<const std::uint8_t> data);
std::expected<Texture, std::string> LoadCubemap(const std::array<std::filesystem::path, 6>& faces);

#endif