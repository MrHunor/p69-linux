/*
 * Copyright (C) 2026 MrHunor
 * LICENSE:GNU General Public License v3 (GPLv3)
 */
#include "../utils/defs.h"
#include <string>
std::string GetCurrentPlayingInfo();
std::string DownloadVideo(const std::string &videoName, int resoltuinH);
int getVideoHeight(const std::string &videoName);
void runVideoLoop(stateClass &state);