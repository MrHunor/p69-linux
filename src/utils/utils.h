#include <source_location>
#include <string>
std::string executeCommand(const std::string& command);
void InvalidInputMessage(const std::string &details, std::source_location location = std::source_location::current());
std::string extractID(const std::string& filename);
std::string findFileByID(const std::string& dirPath, const std::string& id);
void restartSong();