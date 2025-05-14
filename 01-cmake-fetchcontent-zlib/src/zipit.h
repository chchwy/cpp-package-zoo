#pragma once

#include <filesystem>

bool zip_folder(const std::filesystem::path& folder_path, const std::filesystem::path& zip_path);

std::string path_string(const std::filesystem::path& path);