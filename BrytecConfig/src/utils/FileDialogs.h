#pragma once

#include <filesystem>

class FileDialogs {
    FileDialogs() = default;

public:
    static std::filesystem::path OpenFile(const char* filter, std::filesystem::path defaultPath);
    static std::filesystem::path PickFolder(std::filesystem::path defaultPath);
    static std::filesystem::path SaveFile(const char* filter, std::filesystem::path defaultPath = std::filesystem::path{});
};