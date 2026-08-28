#include <filesystem>
#include <fstream>
#include <format>
#include <source_location>
#include <chrono>
#include <iostream>

class stateClass
{
public:
std::ofstream logFile{"log.txt"};
    int verbose;
    bool deleteOverflow;
    // LINK ../../docs/core.md:6
    void out(const std::string &output, int importance, std::source_location location = std::source_location::current())
    {
        if (importance > verbose)
            return;
        std::string message = "";
        if (verbose >= 4)
        {
            auto now = std::chrono::system_clock::now();
            message += std::format("@{} -> ", now);
        }
        if (verbose >= 4)message += std::format("{}->", location.function_name());
        message += output;
         std::cout << message;
        std::cout << std::endl;
        if(logFile.is_open()) logFile<<message<<std::endl;
    }
};