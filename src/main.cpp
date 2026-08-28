
#include <SDL3/SDL_video.h>
#include <iostream>
#include <filesystem>
#include <string>
#include <ostream>
#include "utils/utils.h"
#include "vid/vid.h"
#include "SDL3/SDL.h"
#include "vlc/vlc.h"
#include "audio/audio.h"
#include "utils/defs.h"
#define DEFAULT_SAMPLE_RATE 44100


int main(int argc, char *argv[])
{
   stateClass state;
   state.verbose=4;
   
   state.out("Quering current playing Info..",4);
   std::string current = GetCurrentPlayingInfo();
   state.out("Current Playing:"+current,4);

   state.out("Downloading Video...",4);
   std::string videoName = DownloadVideo(current); 
   state.out("Downloaded video name:"+videoName,4);

   state.out("Attempting to restart song...",4);
   restartSong();

   state.out("Capturing 5 seconds of audio into sample.wav",4);
   CaptureAudio(5,"sample.wav");

   state.out("Converting "+videoName+" and sample.wav to mono ",4);
   std::string videoMusicIsolated = "VideoSample.wav";
   ConvertToMono(videoName,videoMusicIsolated, DEFAULT_SAMPLE_RATE);
   ConvertToMono("sample.wav","sample.wav",DEFAULT_SAMPLE_RATE);
   
   state.out("Loading audio files info memory vectors..",4);
   std::vector<float> origin = loadWavMonoToVector(videoMusicIsolated);
   std::vector<float> sample = loadWavMonoToVector("sample.wav");
   state.out("Loaded.\n origin.size():"+std::to_string(origin.size())+"\nsample.size()"+std::to_string(sample.size()),4);
   
   state.out("Attempting to match clips",4);
   fftMatchResult result = findMatch(origin,sample,DEFAULT_SAMPLE_RATE);
   std::cout<<"Offset in samples:"<<result.offsetInSamples<<"\n Offset in Seconds:"<<result.offsetInSeconds<<"\n BestMatchScore:"<<result.Score<<std::endl;
   

//displaying op

    bool running = true;
SDL_Event event;

// SDL
SDL_SetHint(SDL_HINT_VIDEO_DRIVER,"x11");

if (!SDL_Init(SDL_INIT_VIDEO))InvalidInputMessage("Failed to initialise SDL");
state.out("Using SDL Video Driver:"+std::string(SDL_GetCurrentVideoDriver()),4);

SDL_Window* window =SDL_CreateWindow("P69", 1280, 720, SDL_WINDOW_RESIZABLE);

if (!window)InvalidInputMessage("Failed to create SDL window");

// LIBVLC
libvlc_instance_t* vlc = libvlc_new(0, NULL);

if (!vlc)InvalidInputMessage("Failed to initialise libVLC");

libvlc_media_t* media =libvlc_media_new_path(vlc, videoName.c_str());

if (!media)InvalidInputMessage("Failed to create VLC media");

libvlc_media_player_t* mediaplayer =libvlc_media_player_new_from_media(media);

libvlc_media_release(media);

if (!mediaplayer)InvalidInputMessage("Failed to create VLC media player");

// Get SDL native window properties
//TODO have a look if this can be done nicer
SDL_PropertiesID props = SDL_GetWindowProperties(window);

void* x11Window = SDL_GetPointerProperty(props,SDL_PROP_WINDOW_X11_WINDOW_NUMBER,NULL);

if (x11Window)libvlc_media_player_set_xwindow(mediaplayer,(uint32_t)(uintptr_t)x11Window);
else InvalidInputMessage("Failed to fetch X11 Window ID");



// Restart song before opening video
restartSong();

libvlc_media_player_play(mediaplayer);

// Event loop
while (running)
{
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
            running = false;
    }
}

// Cleanup
libvlc_media_player_stop(mediaplayer);
libvlc_media_player_release(mediaplayer);
libvlc_release(vlc);

SDL_DestroyWindow(window);
SDL_Quit();

return 0;
}