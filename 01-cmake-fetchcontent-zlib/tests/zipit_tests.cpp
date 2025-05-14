#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <string>



// Include your implementation headers here
#include "../src/zipit.h"

TEST(ZipTest, SimpleZip) {
    // Setup a test directory and files
    std::filesystem::path testDir = std::filesystem::temp_directory_path() / "zip_test";
    std::filesystem::create_directories(testDir);
    
    // Create a test file
    std::ofstream testFile(testDir / L"許功蓋.txt");
    testFile << "Hello, world!";
    testFile.close();
    
    // Call your zip function - replace with your actual function call
    std::filesystem::path zipPath = std::filesystem::temp_directory_path() / L"test_許功蓋.zip";
    bool result = zip_folder(testDir, zipPath);  // Call your function here
    
    // Assert that zip was created
    EXPECT_TRUE(result);
    EXPECT_TRUE(std::filesystem::exists(zipPath));
    
    // Cleanup
    std::filesystem::remove_all(testDir);
    std::filesystem::remove(zipPath);
}

// Add more tests here