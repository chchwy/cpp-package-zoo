
#include <iostream>
#include <spdlog/spdlog.h>
#include "zipit.h"


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


#ifdef _WIN32
int wmain3(int argc, wchar_t* argv[]) {
    // Enable UTF-8 in Windows console
    enable_utf8_console();
#else
int main(int argc, char* argv[]) {
#endif
    spdlog::info("Welcome to spdlog!");
    spdlog::info("This is a test for zipping folders with CJK characters in the name.");

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
        std::cout << "Folder does not exist: " << path_string(folder) << "\n";
        return 1;
    }

    std::cout << "Compressing directory: " << path_string(folder) << "\n";

    bool ok = zip_folder(folder, zipfile);

    if (ok && std::filesystem::exists(zipfile)) {
        std::cout << "Successfully created zip: " << path_string(zipfile) << std::endl;
        spdlog::info("Confirmed the zip exists {}", path_string(zipfile));
    } else {
        std::cerr << "Failed to create zip: " << path_string(zipfile) << std::endl;
        return 1;
    }

    return 0;
}
