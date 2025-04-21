#include <iostream>
#include <filesystem>

#include "zlib.h"
#include "contrib/minizip/zip.h"

bool compressDirectory(const std::filesystem::path& dirPath, const std::filesystem::path& zipPath) {
    // Placeholder for the actual compression logic
    std::cout << "Compressing directory: " << dirPath << " to " << zipPath << std::endl;

    zipOpen2_64(zipPath.c_str(), APPEND_STATUS_CREATE, nullptr, nullptr);
    return true; // Assume success for this placeholder
}

int main(int, char**){
    std::cout << "Hello!\n";

    compressDirectory("C:/Temp/cjk/許功蓋/", "test.zip");
}
