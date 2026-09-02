/*
 * Copyright (C) 2026 MrHunor
 * LICENSE:GNU General Public License v3 (GPLv3)
 */
#include <string>
#include "../utils/defs.h"
std::string GetCurrentPlayingInfo();
std::string DownloadVideo(const std::string& videoName_);
void runVideoLoop(stateClass& state);