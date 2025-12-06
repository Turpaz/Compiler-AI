#include "FileReader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/stat.h>

std::string FileReader::readFromArgs(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: compiler <source_file.lang>" << std::endl;
        return "";
    }
    
    std::string filepath = argv[1];
    
    if (!fileExists(filepath)) {
        std::cerr << "Error: File '" << filepath << "' does not exist" << std::endl;
        return "";
    }
    
    std::string code = readFile(filepath);
    if (code.empty()) {
        std::cerr << "Error: Failed to read file '" << filepath << "'" << std::endl;
        return "";
    }
    
    /*std::cout << "=== Compiling: " << filepath << " ===" << std::endl;
    std::cout << "File size: " << getFileSize(filepath) << " bytes" << std::endl;
    std::cout << std::endl;*/
    
    return code;
}

std::string FileReader::readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filepath << "'" << std::endl;
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return buffer.str();
}

bool FileReader::fileExists(const std::string& filepath) {
    struct stat buffer;
    return (stat(filepath.c_str(), &buffer) == 0);
}

size_t FileReader::getFileSize(const std::string& filepath) {
    struct stat stat_buf;
    int rc = stat(filepath.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : 0;
}
