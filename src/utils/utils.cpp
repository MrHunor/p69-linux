#include <iostream>
#include <string>
#include <source_location>
#include <stacktrace>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

std::string extractID(const std::string& filename)
{
    size_t start = filename.rfind('[');
    size_t end = filename.rfind(']');
    
    if (start != std::string::npos && end != std::string::npos && (end - start - 1) == 11) {
        return filename.substr(start + 1, 11);
    }
    return "";
}

void restartSong()
{
system("powershell -c \"(New-Object -ComObject WScript.Shell).SendKeys([char]177)\"");
}

std::string findFileByID(const std::string& dirPath, const std::string& id) {
    for (const auto& entry : fs::directory_iterator(dirPath)) {
        std::string filename = entry.path().string();
        if (filename.find(id) != std::string::npos) {
            return filename; 
        }
    }
    return "";
}
void InvalidInputMessage(const std::string &details, std::source_location location)
{
    std::cout << std::stacktrace::current() << std::endl;
    std::cout << "Filename:" << location.file_name() << std::endl;
    std::cout << "Function:" << location.function_name() << std::endl;
    std::cout << "Line:" << location.line() << std::endl;
    std::cout << "Column:" << location.column() << std::endl;
    if (!details.empty())
    {
        std::cout << "Details provided:" << details << std::endl;
    }
    exit(1);
}
