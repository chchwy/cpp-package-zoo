// zip_folder.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include <cstdio>
#include <ctime>

#include "zip.h" // Include the original minizip's zip.h
#ifdef _WIN32
    #include <fcntl.h>
    #include <io.h>

  #define USEWIN32IOAPI
  #include "iowin32.h"
#endif


constexpr int UTF8_FLAG = (1 << 11); // Bit 11 set means filename is UTF-8 encoded


// Convert Windows wide string (UTF-16) to UTF-8
std::string wstring_to_utf8(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();

    // Get the required buffer size
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
        static_cast<int>(wstr.length()),
        nullptr, 0, nullptr, nullptr);

    std::string utf8_str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.length()),
        &utf8_str[0], size_needed, nullptr, nullptr);

    return utf8_str;
}

std::string path_string(const std::filesystem::path& path) {
#if _WIN32
	// Convert to UTF-8 and print
	std::string utf8_str = wstring_to_utf8(path.wstring());
	return utf8_str;
#else
    return path.string();
#endif
}

bool add_file_to_zip(zipFile zf, const std::filesystem::path& file_path, const std::filesystem::path& relative_path) {
    
    std::ifstream file;
    file.open(file_path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    // Prepare zip file info
    zip_fileinfo zi = {};
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    zi.tmz_date.tm_sec  = now->tm_sec;
    zi.tmz_date.tm_min  = now->tm_min;
    zi.tmz_date.tm_hour = now->tm_hour;
    zi.tmz_date.tm_mday = now->tm_mday;
    zi.tmz_date.tm_mon  = now->tm_mon;
    zi.tmz_date.tm_year = now->tm_year;

    if (ZIP_OK != zipOpenNewFileInZip4(
            zf,
            (const char*)relative_path.u8string().c_str(),
            &zi,
            NULL, 0, // local extra field
            NULL, 0, // global extra field
            NULL, // comment
            Z_DEFLATED, Z_DEFAULT_COMPRESSION, // compression method and level
            0, // raw
            -MAX_WBITS, // windowBits
            DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,  // memLevel and strategy
            NULL, 0, // password and crcForCrypting
            0, // versionMadeBy
            UTF8_FLAG // UTF-8 flag
        )) {
        std::cerr << "Failed to add file to zip: " << relative_path << std::endl;
        file.close();
        return false;
    }

    // Write file contents into the zip
    char buffer[4096];
    while (file) {
		file.read(buffer, sizeof(buffer));
		std::streamsize len = file.gcount();
        if (len > 0) {
            int ok = zipWriteInFileInZip(zf, buffer, len);
			if (ok != ZIP_OK) {
                std::cerr << "Failed to write file: " << relative_path << std::endl;
                zipCloseFileInZip(zf);
                file.close();
                return false;
            }
        }
    }

    zipCloseFileInZip(zf);
    file.close();
    return true;
}

bool zip_folder(const std::filesystem::path& folder_path, const std::filesystem::path& zip_path) {

#ifdef _WIN32
    zlib_filefunc64_def ffunc64;
    fill_win32_filefunc64W(&ffunc64);
    std::wstring wpath = zip_path.wstring();

    zipFile zf = zipOpen2_64(wpath.c_str(),APPEND_STATUS_CREATE,NULL,&ffunc64);
#endif
    if (!zf) {
        std::cerr << "Failed to create zip file: " << zip_path << std::endl;
        return false;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(folder_path)) {
        if (!entry.is_directory()) {
            std::filesystem::path file_path = entry.path();
            std::filesystem::path relative_path = std::filesystem::relative(entry.path(), folder_path);
            // Always use '/' inside zip files
            //std::replace(relative_path.begin(), relative_path.end(), '\\', '/');

            if (!add_file_to_zip(zf, file_path, relative_path)) {
                std::cerr << "Error adding file: " << file_path << std::endl;
            }
        }
    }

    zipClose(zf, NULL);
    return true;
}

#ifdef _WIN32
// Enable UTF-8 console on Windows
void enable_utf8_console() {
    // Set console output CP to UTF-8
    SetConsoleOutputCP(CP_UTF8);

    // Enable UTF-8 console features
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#endif

int wmain(int argc, wchar_t* argv[]) {

#ifdef _WIN32
    // Enable UTF-8 in Windows console
    enable_utf8_console();
#endif

    if (argc < 3) {
		std::cerr << "Usage: zipit <folder_to_zip> <output_zip_file>" << std::endl;
		return 1;
    }

    // Example paths — can contain Unicode (CJK)
    //std::filesystem::path folder {u8"C:/temp/cjk/許功蓋"};  // Folder to zip (make sure it exists!)
    //std::filesystem::path zipfile {u8"C:/temp/許功蓋输出壓縮包.zip"}; // Output zip file name

	std::filesystem::path folder{ argv[1] };  // Folder to zip (make sure it exists!)
	std::filesystem::path zipfile{ argv[2] }; // Output zip file name

    if (!std::filesystem::exists(folder)) {
        std::cout << "Folder does not exist.\n";
    }

    std::cout << "File path received: " << path_string(folder) << "\n";

    try {
        //std::cout << "Compressing folder: " << folder.string() << std::endl;
        //std::cout << "Output zip file: " << zipfile << std::endl;
    } catch (const std::system_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    bool ok = zip_folder(folder, zipfile);
    if (ok && std::filesystem::exists(zipfile)) {
        std::cout << "Successfully created zip: " << path_string(zipfile) << std::endl;
    } else {
        std::cerr << "Failed to create zip: " << path_string(zipfile) << std::endl;
        return 1;
    }

    if (std::filesystem::exists(zipfile)) {
        std::cout << "Yes!!!\n" << std::endl;
    }

    return 0;
}
