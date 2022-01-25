#pragma once

#include <filesystem>

class FileDialogs
{
	FileDialogs() = default;

public:
	static std::filesystem::path OpenFile(const char* filter, const char* defaultPath = NULL);
	static std::filesystem::path SaveFile(const char* filter, const char* defaultPath = NULL);
};