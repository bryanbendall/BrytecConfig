#include "FileDialogs.h"

#include <nfd.h>

namespace Brytec {

std::filesystem::path FileDialogs::OpenFile(const char* filter, std::filesystem::path defaultPath)
{
    nfdchar_t* outPath = NULL;
    nfdresult_t result = NFD_OpenDialog(filter, 
        defaultPath.empty() ? NULL : defaultPath.string().c_str(), 
        &outPath);

    if (result != NFD_OKAY)
        return std::filesystem::path();

    std::filesystem::path path = outPath;
    free(outPath);

    return path;
}

std::filesystem::path FileDialogs::PickFolder(std::filesystem::path defaultPath)
{
    nfdchar_t* outPath = NULL;
    nfdresult_t result = NFD_PickFolder(defaultPath.empty() ? NULL : defaultPath.string().c_str(), &outPath);

    if (result != NFD_OKAY)
        return std::filesystem::path();

    std::filesystem::path path = outPath;
    free(outPath);

    return path;
}

std::filesystem::path FileDialogs::SaveFile(const char* filter, std::filesystem::path defaultPath)
{
    nfdchar_t* outPath = NULL;
    nfdresult_t result = NFD_SaveDialog(filter, 
        defaultPath.empty() ? NULL : defaultPath.string().c_str(), 
        &outPath);

    if (result != NFD_OKAY)
        return std::filesystem::path();

    std::filesystem::path path = outPath;
    free(outPath);

    return path;
}

}