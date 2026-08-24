#include <iostream>
#include "../utils/utils.h"

std::string GetCurrentPlayingInfo()
{
    /*
    char buffer[256];
    std::string result = "";
//open process pipe for reading r 
    FILE* pipe = _popen("powershell -Command \"[Windows.Media.Control.GlobalSystemMediaTransportControlsSessionManager,Windows.Media.Control,ContentType=WindowsRuntime] | Out-Null; Add-Type -AssemblyName System.Runtime.WindowsRuntime; $asTask = ([System.WindowsRuntimeSystemExtensions].GetMethods() | Where-Object { $_.Name -eq 'AsTask' -and $_.GetParameters().Count -eq 1 -and $_.GetParameters()[0].ParameterType.Name -eq 'IAsyncOperation`1' })[0]; $mgr = $asTask.MakeGenericMethod([Windows.Media.Control.GlobalSystemMediaTransportControlsSessionManager]).Invoke($null, @([Windows.Media.Control.GlobalSystemMediaTransportControlsSessionManager]::RequestAsync())); $session = $mgr.GetAwaiter().GetResult().GetCurrentSession(); if ($session) { $props = $asTask.MakeGenericMethod([Windows.Media.Control.GlobalSystemMediaTransportControlsSessionMediaProperties]).Invoke($null, @($session.TryGetMediaPropertiesAsync())).GetAwaiter().GetResult(); Write-Output \\\"$($props.Artist) - $($props.Title)\\\" }\"","r");
    if(!pipe) InvalidInputMessage("Coudnt open Powershell pipe");

    //Read output till no more is remaining
    while(fgets(buffer,sizeof(buffer),pipe)!=nullptr)
    {
        result += buffer;
    }
    _pclose(pipe);

    return result;
*/
return "EMPTY";
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