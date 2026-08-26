
#include <iostream>
#include <filesystem>
#include "utils/utils.h"
#include "vid/vid.h"
#include "SDL3/SDL.h"
#include "vlc/vlc.h"
#include "audio/audio.h"

int main(int argc, char *argv[])
{
   std::string current = GetCurrentPlayingInfo();
    CaptureAudio(5,"\""+current+".wav\"");
  /*    bool running = true;
    SDL_Event event;
    std::cout << GetCurrentPlayingInfo() << std::endl;
    std::filesystem::path filePath = findFileByID(std::filesystem::current_path().string(), extractID(DownloadVideo(GetCurrentPlayingInfo())));
    std::string filePathString = filePath.string();
    // SDL
    if (!SDL_Init(SDL_INIT_VIDEO))
        InvalidInputMessage("Failed to Initilise SDL");
    SDL_Window *window = SDL_CreateWindow("P69", 1280, 720, SDL_WINDOW_RESIZABLE);

    // LIBVLC
    libvlc_instance_t *vlc = libvlc_new(0, NULL);
    libvlc_media_t *media = libvlc_media_new_path(vlc, filePathString.c_str());
    libvlc_media_player_t *mediaplayer = libvlc_media_player_new_from_media(media);
    libvlc_media_release(media);

    SDL_PropertiesID props = SDL_GetWindowProperties(window);

    void *hwnd = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    if (hwnd)
        libvlc_media_player_set_hwnd(mediaplayer, hwnd);
    else
        InvalidInputMessage("Failed to fetch/extract Window HWND");

    // restart song has to come before opening the video file otherwise it is likely the video file gets treated as the media (offset i know but the beat matching will fix this )
    restartSong();
    libvlc_media_player_play(mediaplayer);

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
        if(event.type == SDL_EVENT_QUIT) running = false;
        }
    }
//cleanup
//vlc
    libvlc_media_player_stop(mediaplayer);
libvlc_media_player_release(mediaplayer);
libvlc_release(vlc);

//SDL
SDL_DestroyWindow(window);
SDL_Quit();
*/

return 0;
}