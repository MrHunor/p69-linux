/*
 * Copyright (C) 2026 MrHunor
 * LICENSE:GNU General Public License v3 (GPLv3)
 */
#include <iostream>
#include <portaudio.h>
#include <string>
#include <source_location>
#include <stacktrace>
#include <filesystem>
#include <string>
#include <thread>
#include <chrono>
#include <unistd.h>
#include <limits.h>
#include "utils.h"
namespace fs = std::filesystem;


std::filesystem::path getExecutableDir()
{
    char buffer[PATH_MAX];

    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);

    if (len == -1)
        InvalidInputMessage("Failed to get executable directory.");

    buffer[len] = '\0';

    return std::filesystem::path(buffer).parent_path();
}



std::string removeNewLineAndReturnCharacters(const std::string& inputString)
{
std::string retval= inputString;
  while (!retval.empty() &&
          (retval.back() == '\n' || retval.back() == '\r'))
    {
        retval.pop_back();//delete last character
    }
return retval;
}


std::string getRidOfESCCharactersinAstrics(const std::string& str)
{
    std::string result = "'";
    
    for (char c : str)
    {
        if (c == '\'')
            result += "'\\''";
        else
            result += c;
    }

    result += "'";
    return result;
}

std::string executeCommand(const std::string& command)
{
    FILE* pipe = popen(command.c_str(),"r");

char buffer[256];
std::string result;

while (fgets(buffer, sizeof(buffer), pipe)) //this works because fgets returns a pointer to the data, and if no data is available it return nullptr
{
    result += buffer;
}
pclose(pipe);

return result;
}

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

    executeCommand("playerctl pause");
    executeCommand("playerctl position 0");

    // Wait until the seek has actually taken effect.
    while (true)
    {
        std::string output = executeCommand("playerctl position");

    
            double position = std::stod(output);

            if (position <= 0.01)
                break;
        
        
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    executeCommand("playerctl play");
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
    //yes i thought of making this state out for colour but that would require passing state (which is not always present), or make state global which is not pretty
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
