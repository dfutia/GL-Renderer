#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "File.h"


Texture::Texture() : id(0)
{
	glGenTextures(1, &id);
}

Texture::Texture(const Texture& other) : id(other.id)
{
}

//Texture::Texture(const Image& image, Format format) : id(0)
//{
//	glGenTextures(1, &id);
//
//	auto [width, height] = image.GetDimensions();
//	int channels = image.GetChannels();
//
//	// Determine the OpenGL format based on channels
//	GLenum glFormat;
//	GLenum internalFormat;
//	switch (channels)
//	{
//	case 1:
//		glFormat = GL_RED;
//		internalFormat = GL_RED;
//		break;
//	case 2:
//		glFormat = GL_RG;
//		internalFormat = GL_RG;
//		break;
//	case 3:
//		glFormat = GL_RGB;
//		internalFormat = GL_RGB;
//		break;
//	case 4:
//	default:
//		glFormat = GL_RGBA;
//		internalFormat = GL_RGBA;
//		break;
//	}
//
//	glBindTexture(GL_TEXTURE_2D, id);
//	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, glFormat, GL_UNSIGNED_BYTE, image.GetData());
//
//	// Set default wrapping and filtering
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//	glGenerateMipmap(GL_TEXTURE_2D);
//}

Texture::~Texture()
{
	//glDeleteTextures(1, &id);
}

const Texture& Texture::operator=(const Texture& other)
{
	if (this != &other)
	{
		id = other.id;
	}
	return *this;
}

void Texture::Image2D(const void* data, GLenum type, GLenum format, unsigned int width, unsigned int height, GLenum internalFormat)
{
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);
}

void Texture::SetWrapping(WrapMode wrapS)
{
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
}

void Texture::SetWrapping(WrapMode wrapS, WrapMode wrapT)
{
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
}

void Texture::SetWrapping(WrapMode wrapS, WrapMode wrapT, WrapMode wrapR)
{
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrapR);
}

void Texture::SetFilters(FilterMode minFilter, FilterMode magFilter)
{
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

void Texture::SetBorderColor()
{
	glBindTexture(GL_TEXTURE_2D, id);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
}

void Texture::GenerateMipmaps()
{
	glBindTexture(GL_TEXTURE_2D, id);
	glGenerateMipmap(GL_TEXTURE_2D);
}

Texture LoadTexture(const std::string& filepath)
{
	std::vector<std::uint8_t> fileData = ReadBinaryFile(filepath);
	if (fileData.empty())
		throw std::runtime_error("Failed to load: " + filepath);

	int width, height, channels;
	unsigned char* pixels = stbi_load_from_memory(
		fileData.data(), static_cast<int>(fileData.size()),
		&width, &height, &channels, 0);

	if (!pixels)
		throw std::runtime_error("Failed to decode: " + filepath);

	GLenum format;
	switch (channels)
	{
	case 1: format = GL_RED;  break;
	case 2: format = GL_RG;   break;
	case 3: format = GL_RGB;  break;
	default: format = GL_RGBA; break;
	}

	Texture texture;
	//texture.width = width;
	//texture.height = height;
	//texture.channels = channels;
	texture.Image2D(pixels, GL_UNSIGNED_BYTE, format, width, height, format);
	texture.SetWrapping(Texture::Repeat, Texture::Repeat);
	texture.SetFilters(Texture::LinearMipmapLinear, Texture::Linear);
	texture.GenerateMipmaps();

	stbi_image_free(pixels);
	return texture;
}

Texture LoadTexture(const unsigned char* data, int byteLength)
{
	int width, height, channels;
	unsigned char* pixels = stbi_load_from_memory(
		data, byteLength,
		&width, &height, &channels, 0);
	if (!pixels)
		throw std::runtime_error("Failed to decode texture from memory");
	GLenum format;
	switch (channels)
	{
	case 1: format = GL_RED;  break;
	case 2: format = GL_RG;   break;
	case 3: format = GL_RGB;  break;
	default: format = GL_RGBA; break;
	}
	Texture texture;
	texture.Image2D(pixels, GL_UNSIGNED_BYTE, format, width, height, format);
	texture.SetWrapping(Texture::Repeat, Texture::Repeat);
	texture.SetFilters(Texture::LinearMipmapLinear, Texture::Linear);
	texture.GenerateMipmaps();
	stbi_image_free(pixels);
	return texture;
}

Texture LoadCubemap(const std::array<std::string, 6>& faces)
{
	Texture texture;
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	for (int i = 0; i < 6; i++)
	{
		auto fileData = ReadBinaryFile(faces[i]);
		int width, height, channels;
		unsigned char* pixels = stbi_load_from_memory(
			fileData.data(), (int)fileData.size(),
			&width, &height, &channels, 0);

		GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
		stbi_image_free(pixels);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return texture;
}