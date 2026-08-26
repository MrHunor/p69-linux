#include <vector>
#include <string>
#include <portaudio.h>
#include "../utils/utils.h"
#include <sndfile.h>
#include <format>
#include <cstdlib>
//ai assisted
void CaptureAudio(double durationSeconds, const std::string& outputFile) {

    std::string command = std::format("ffmpeg -f pulse -i \"$(pactl get-default-sink).monitor\" -t {} {}",durationSeconds,outputFile);
    system(command.c_str());
}