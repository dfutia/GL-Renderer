#ifndef FILE_H
#define FILE_H

#include <SDL.h>

#include "PCH.h"

inline std::filesystem::path GetRoot(int parentLevels = 4)
{
	char* base = SDL_GetBasePath();
	std::filesystem::path dir(base);
	SDL_free(base);

	for (int i = 0; i < parentLevels; ++i)
	{
		dir = dir.parent_path();
	}

	return dir;
}

inline std::filesystem::path GetMediaPath()
{
	return GetRoot() / "Assets";
}

// used for reading text files such as shaders, scripts, etc.
inline std::string ReadTextFile(const std::filesystem::path& filepath)
{
	std::ifstream file(filepath, std::ios::in | std::ios::binary);
	if (!file.is_open())
	{
		std::println("Failed to open file: {}", filepath.string());
		return "";
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

// used for reading binary files such as models, images, etc.
inline std::vector<std::uint8_t> ReadBinaryFile(const std::filesystem::path& filepath)
{
	std::ifstream file(filepath, std::ios::binary);
	if (!file)
	{
		std::println("Failed to open file: {}", filepath.string());
		return {};
	}

	file.seekg(0, std::ios::end);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	if (size <= 0)
		return {};

	std::vector<std::uint8_t> bytes(static_cast<size_t>(size));
	if (!file.read(reinterpret_cast<char*>(bytes.data()), size))
		return {};

	return bytes;
}


#endif