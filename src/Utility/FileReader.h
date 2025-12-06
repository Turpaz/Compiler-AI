#ifndef FILEREADER_H
#define FILEREADER_H

#include <string>

class FileReader {
public:
    // Read source code from command-line arguments
    // Returns code string (empty on error)
    // Handles all file validation and error messages
    static std::string readFromArgs(int argc, char* argv[]);
    
    // Read entire file content into a string
    // Returns empty string on failure, check with fileExists first
    static std::string readFile(const std::string& filepath);
    
    // Check if file exists
    static bool fileExists(const std::string& filepath);
    
    // Get file size in bytes
    static size_t getFileSize(const std::string& filepath);
};

#endif // FILEREADER_H
