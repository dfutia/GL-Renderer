#include "PCH.h"
#include "Texture.h"
#include "Platform/File.h"
#include <stb_image.h>

Texture::Texture()
{
	glGenTextures(1, &id);
}

Texture::~Texture()
{
	if (id != 0)
		glDeleteTextures(1, &id);
}

Texture::Texture(Texture&& other) noexcept
	: id(other.id), target(other.target)
{
	other.id = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept
{
	if (this != &other)
	{
		if (id != 0)
			glDeleteTextures(1, &id);
		id = other.id;
		target = other.target;
		other.id = 0;
	}
	return *this;
}

void Texture::Image2D(const void* data, GLenum type, GLenum format, unsigned int width, unsigned int height, GLenum internalFormat)
{
	target = GL_TEXTURE_2D;
	glBindTexture(GL_TEXTURE_2D, id);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);
}

void Texture::Image2DMultisample(unsigned int samples, GLenum internalFormat, unsigned int width, unsigned int height)
{
	target = GL_TEXTURE_2D_MULTISAMPLE;
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, id);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_TRUE);
}

void Texture::SetWrapping(WrapMode wrapS)
{
	if (target == GL_TEXTURE_2D_MULTISAMPLE) return;
	glBindTexture(target, id);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapS);
}

void Texture::SetWrapping(WrapMode wrapS, WrapMode wrapT)
{
	if (target == GL_TEXTURE_2D_MULTISAMPLE) return;
	glBindTexture(target, id);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapT);
}

void Texture::SetWrapping(WrapMode wrapS, WrapMode wrapT, WrapMode wrapR)
{
	if (target == GL_TEXTURE_2D_MULTISAMPLE) return;
	glBindTexture(target, id);
	glTexParameteri(target, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(target, GL_TEXTURE_WRAP_T, wrapT);
	glTexParameteri(target, GL_TEXTURE_WRAP_R, wrapR);
}

void Texture::SetFilters(FilterMode minFilter, FilterMode magFilter)
{
	if (target == GL_TEXTURE_2D_MULTISAMPLE) return;
	glBindTexture(target, id);
	glTexParameteri(target, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(target, GL_TEXTURE_MAG_FILTER, magFilter);
}

void Texture::SetBorderColor()
{
	if (target == GL_TEXTURE_2D_MULTISAMPLE) return;
	glBindTexture(target, id);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, borderColor);
}

void Texture::SetBorderColor(float r, float g, float b, float a)
{
	if (target == GL_TEXTURE_2D_MULTISAMPLE) return;
	glBindTexture(target, id);
	float borderColor[] = { r, g, b, a };
	glTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, borderColor);
}

void Texture::GenerateMipmaps()
{
	if (target == GL_TEXTURE_2D_MULTISAMPLE) return;
	glBindTexture(target, id);
	glGenerateMipmap(target);
}

std::expected<Texture, std::string> LoadTexture(const std::filesystem::path& filepath)
{
	std::vector<std::uint8_t> fileData = ReadBinaryFile(filepath.string());
	if (fileData.empty())
	{
		return std::unexpected("Failed to load file: " + filepath.string());
	}

	return LoadTexture(std::span<const std::uint8_t>(fileData));
}

std::expected<Texture, std::string> LoadTexture(std::span<const std::uint8_t> data)
{
	int width, height, channels;
	unsigned char* pixels = stbi_load_from_memory(data.data(), static_cast<int>(data.size()), &width, &height, &channels, 0);

	if (!pixels)
	{

		return std::unexpected(std::string("Failed to decode texture: ") + stbi_failure_reason());
	}

	GLenum format;
	switch (channels)
	{
	case 1:  format = GL_RED;  break;
	case 2:  format = GL_RG;   break;
	case 3:  format = GL_RGB;  break;
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

std::expected<Texture, std::string> LoadCubemap(const std::array<std::filesystem::path, 6>& faces)
{
	Texture texture;
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	for (int i = 0; i < 6; i++)
	{
		std::vector<std::uint8_t> fileData = ReadBinaryFile(faces[i].string());
		if (fileData.empty())
		{
			return std::unexpected("Failed to load cubemap face: " + faces[i].string());
		}

		int width, height, channels;
		unsigned char* pixels = stbi_load_from_memory(fileData.data(), static_cast<int>(fileData.size()),&width, &height, &channels, 0);

		if (!pixels)
		{
			return std::unexpected("Failed to decode cubemap face: " + faces[i].string() + " - " + stbi_failure_reason());
		}

		GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, pixels);
		stbi_image_free(pixels);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return texture;
}