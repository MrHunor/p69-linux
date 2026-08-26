#include <iostream>
#include "../utils/utils.h"

std::string GetCurrentPlayingInfo()
{
return executeCommand(std::string("playerctl metadata --format '{{ artist }} - {{ title }}'"));
}

std::string DownloadVideo(const std::string& videoName)
{
/*
    std::string query = videoName;
    while (!query.empty() &&
          (query.back() == '\n' || query.back() == '\r'))
    {
        query.pop_back();
    }

 std::string cmd =
    "yt-dlp -S \"vcodec:h264,res:720\" "
    "--print after_move:filepath "
    "\"ytsearch:" + query + "\"";

    FILE* pipe = _popen(cmd.c_str(), "r");
    if (!pipe)
        return "";

    char buffer[512];
    std::string filename;

    while (fgets(buffer, sizeof(buffer), pipe))
        filename += buffer;

    _pclose(pipe);

    while (!filename.empty() &&
          (filename.back() == '\n' || filename.back() == '\r'))
    {
        filename.pop_back();
    }

    return filename;
    */
    return "EMPTY";
}