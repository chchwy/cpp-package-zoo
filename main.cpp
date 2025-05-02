#include <iostream>
#include <fstream>
#include <filesystem>
#include <iomanip>

#include "zlib.h"
#include "contrib/minizip/zip.h"
#ifdef _WIN32
#include "contrib/minizip/iowin32.h"
#endif

bool compressDirectory(const std::filesystem::path& dirPath, const std::filesystem::path& zipPath) {
    
    std::wcout << L"Compressing directory: " << dirPath.wstring() << L" to " << zipPath.wstring() << std::endl;

    zipFile zp;
#ifdef _WIN32
    zlib_filefunc64_def filefuncs;
    fill_win32_filefunc64W(&filefuncs);
    zp = zipOpen2_64(zipPath.wstring().c_str(), APPEND_STATUS_CREATE, nullptr, &filefuncs);
#else 
    zp = zipOpen2_64(zipPath.string().c_str(), APPEND_STATUS_CREATE, nullptr, nullptr);
#endif
    
    if (!zp) {
        std::cerr << "Error opening zip file: " << zipPath << std::endl;
        return false;
    }
    
    // For tracking the relative path
    std::error_code ec;

    // Walk through all files in the directory recursively
    for (const auto& entry : std::filesystem::recursive_directory_iterator(dirPath, ec)) {
        
        std::cerr << "Accessing: " << entry.path() << std::endl;
        if (ec) {
            std::cerr << "Error: " << entry.path() << " - " << ec.message() << std::endl;
            continue;
        }

        if (!entry.is_regular_file()) {
            continue;  // Skip directories and other non-regular files
        }

        // Calculate relative path for zip entry
        std::string relativePath;
#ifdef _WIN32
        // On Windows, convert to UTF-8 using Windows API
        std::filesystem::path relPath = entry.path().lexically_relative(dirPath);
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, relPath.c_str(), -1, NULL, 0, NULL, NULL);
        std::vector<char> utf8Path(size_needed);
        WideCharToMultiByte(CP_UTF8, 0, relPath.c_str(), -1, utf8Path.data(), size_needed, NULL, NULL);
        relativePath = std::string(utf8Path.data());
#else
        // On other platforms, using string() is typically fine as it's usually UTF-8
        relativePath = entry.path().lexically_relative(dirPath).string();
#endif

        // Replace Windows backslashes with forward slashes for zip format
        std::replace(relativePath.begin(), relativePath.end(), '\\', '/');

        // Open file to compress
        std::ifstream file(entry.path(), std::ios::binary);
        if (!file) {
            std::cerr << "Failed to open file: " << entry.path() << std::endl;
            continue;
        }

        // Get file size
        auto fileSize = std::filesystem::file_size(entry.path());
        
        // Read file content into buffer
        std::vector<char> buffer(fileSize);
        file.read(buffer.data(), fileSize);
        file.close();

        // Setup zip parameters
        zip_fileinfo zfi = {};
        auto filetime = std::filesystem::last_write_time(entry.path());
        auto sctp = std::chrono::clock_cast<std::chrono::system_clock>(filetime);
        auto tt = std::chrono::system_clock::to_time_t(sctp);
        std::tm* ltm = std::localtime(&tt);
        
        if (ltm) {
            zfi.tmz_date.tm_year = ltm->tm_year;
            zfi.tmz_date.tm_mon = ltm->tm_mon;
            zfi.tmz_date.tm_mday = ltm->tm_mday;
            zfi.tmz_date.tm_hour = ltm->tm_hour;
            zfi.tmz_date.tm_min = ltm->tm_min;
            zfi.tmz_date.tm_sec = ltm->tm_sec;
        }

        /*
        extern int ZEXPORT zipOpenNewFileInZip4_64(zipFile file, const char* filename, const zip_fileinfo* zipfi,
                                           const void* extrafield_local, uInt size_extrafield_local,
                                           const void* extrafield_global, uInt size_extrafield_global,
                                           const char* comment, int method, int level, int raw,
                                           int windowBits,int memLevel, int strategy,
                                           const char* password, uLong crcForCrypting,
                                           uLong versionMadeBy, uLong flagBase, int zip64) {
                                           */
        // Create new file in zip
        constexpr int enableUTF8 = 1 << 11; // UTF-8 filename encoding flag
        int ok = zipOpenNewFileInZip4_64(zp, relativePath.c_str(), &zfi,
                    nullptr, 0, // extrafield_local, size_extrafield_local
                    nullptr, 0, // extrafield_global, size_extrafield_global
                    nullptr, // comment
                    Z_DEFLATED, 
                    Z_DEFAULT_COMPRESSION, 
                    0, // raw
                    -MAX_WBITS,
                    DEF_MEM_LEVEL, 
                    Z_DEFAULT_STRATEGY, 
                    nullptr, // password
                    0, // crcForCrypting
                    0, // versionMadeBy
                    enableUTF8, // flagBase
                    0); // zip64
        if (ok != ZIP_OK) {
            std::cerr << "Error creating zip entry: " << relativePath << std::endl;
            continue;
        }

        // Write content to zip
        if (zipWriteInFileInZip(zp, buffer.data(), static_cast<unsigned int>(fileSize)) != ZIP_OK) {
            std::cerr << "Error writing zip entry: " << relativePath << std::endl;
        }

        // Close the current file in zip
        zipCloseFileInZip(zp);
        
        std::cout << "Added: " << relativePath << " (" << fileSize << " bytes)" << std::endl;
    }

    zipClose(zp, nullptr); 

    return true; // Assume success for this placeholder
}

int main(int, char**) {
    std::cout << "Hello!\n";

    std::filesystem::path source_directory {L"C:/Temp/cjk/許功蓋/"};
    std::filesystem::path dest_archive {L"C:/Temp/許功蓋.zip"};

    bool ok = compressDirectory(source_directory, dest_archive);
	return (ok == true) ? 0 : 1;
}
