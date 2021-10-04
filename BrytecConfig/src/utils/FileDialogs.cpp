#include "FileDialogs.h"

#include <nfd.h>

std::filesystem::path FileDialogs::OpenFile(const char* filter)
{
	nfdchar_t* outPath = NULL;
	nfdresult_t result = NFD_OpenDialog(filter, NULL, &outPath);

	if(result != NFD_OKAY)
		return std::filesystem::path();

	std::filesystem::path path = outPath;
	free(outPath);

	return path;
}

std::filesystem::path FileDialogs::SaveFile(const char* filter, const char* defaultPath)
{
	nfdchar_t* outPath = NULL;
	nfdresult_t result = NFD_SaveDialog(filter, defaultPath, &outPath);

	if(result != NFD_OKAY)
		return std::filesystem::path();

	std::filesystem::path path = outPath;
	free(outPath);

	return path;
}
