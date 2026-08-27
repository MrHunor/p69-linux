#include <iostream>
#include "../utils/utils.h"

std::string GetCurrentPlayingInfo()
{
return executeCommand(std::string("playerctl metadata --format '{{ artist }} - {{ title }}'"));
}

std::string DownloadVideo(const std::string& videoName)
{

    std::string query = videoName;
    query=removeNewLineAndReturnCharacters(query);

 std::string cmd =
    "yt-dlp -S \"vcodec:h264,res:720\" "
    "--print after_move:filepath "
    "\"ytsearch:" + query + "\"";

    
 std::string retval = executeCommand(cmd);
  

    return removeNewLineAndReturnCharacters(retval);
    
}