#include <string>
#include "../utils/defs.h"
std::string GetCurrentPlayingInfo();
std::string DownloadVideo(const std::string& videoName_);
void runVideoLoop(stateClass& state);