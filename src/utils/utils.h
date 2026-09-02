/*
 * Copyright (C) 2026 MrHunor
 * LICENSE:GNU General Public License v3 (GPLv3)
 */
#include <source_location>
#include <string>
#include <filesystem>
std::filesystem::path getExecutableDir();
std::string removeNewLineAndReturnCharacters(const std::string& inputString);
std::string executeCommand(const std::string& command);
void InvalidInputMessage(const std::string &details, std::source_location location = std::source_location::current());
std::string extractID(const std::string& filename);
std::string findFileByID(const std::string& dirPath, const std::string& id);
void restartSong();
std::string getRidOfESCCharactersinAstrics(const std::string& str);