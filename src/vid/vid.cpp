#include <cstdlib>
#include <iostream>
#include "../utils/utils.h"
#include "SDL3/SDL.h"
#include "../audio/audio.h"
#include "../utils/defs.h"
#include "vid.h"
#include "vlc/vlc.h"
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <iostream>
#include <ostream>
#include <string>
#include <vlc/libvlc.h>
#include <vlc/libvlc_media.h>
std::string GetCurrentPlayingInfo()
{
return executeCommand(std::string("playerctl metadata --format '{{ artist }} - {{ title }}'"));
}

void runVideoLoop()
{
    bool running = true;
  SDL_Event event;

  SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "x11");

  if (!SDL_Init(SDL_INIT_VIDEO))
    InvalidInputMessage("Failed to initialise SDL: " +
                        std::string(SDL_GetError()));

  SDL_Window *window = SDL_CreateWindow("P69", 1280, 720, SDL_WINDOW_RESIZABLE);

  if (!window)
    InvalidInputMessage("Failed to create SDL window: " +
                        std::string(SDL_GetError()));

  stateClass state;
  state.verbose = 4;

  while (running == true) {
    state.out("Quering current playing Info..", 4);
    const std::string current = GetCurrentPlayingInfo();
    state.out("Current Playing:" + current, 4);

    state.out("Downloading Video...", 4);
    std::string videoName = DownloadVideo(current);
    state.out("Downloaded video name:" + videoName, 4);

    state.out("Attempting to restart song...", 4);
    restartSong();

    state.out("Capturing 5 seconds of audio into sample.wav", 4);
    CaptureAudio(5, "sample.wav");

    state.out("Converting " + videoName + " and sample.wav to mono ", 4);
    std::string videoMusicIsolated = "VideoSample.wav";
    ConvertToMono(videoName, videoMusicIsolated, DEFAULT_SAMPLE_RATE);
    ConvertToMono("sample.wav", "sample.wav", DEFAULT_SAMPLE_RATE);

    state.out("Loading audio files info memory vectors..", 4);
    std::vector<float> origin = loadWavMonoToVector(videoMusicIsolated);
    std::vector<float> sample = loadWavMonoToVector("sample.wav");
    state.out("Loaded.\n origin.size():" + std::to_string(origin.size()) +
                  "\nsample.size()" + std::to_string(sample.size()),
              4);

    state.out("Attempting to match clips", 4);
    fftMatchResult result = findMatch(origin, sample, DEFAULT_SAMPLE_RATE);
    state.out("Offset in samples:" + std::to_string(result.offsetInSamples)
              + "\n Offset in Seconds:" + std::to_string(result.offsetInSeconds)
              + "\n BestMatchScore:" + std::to_string(result.Score),4);
    if (result.Score < 0.5)
      state.out("No senseful correlation found.", 4);
    state.out("Removing audio from video file...", 4);
    RemoveAudio(videoName, videoName);

    // displaying op
//    std::string pluginPath = std::string(getExecutableDir())+"/plugins";
//    setenv("VLC_PLUGIN_PATH",pluginPath.c_str(), 1);
    
    const char *args[] = {
    "--verbose=4"

    };

    libvlc_instance_t *vlc =
    libvlc_new(sizeof(args) / sizeof(args[0]), args);

    if (!vlc)
      {
      if(libvlc_errmsg()!=0) InvalidInputMessage("Failed to initialise libVLC. Libvlc error message: "+std::string(libvlc_errmsg()));
      else InvalidInputMessage("Failed to initialise vlc; there is no vlc error message to provide");
      }

    libvlc_media_t *media = libvlc_media_new_path(vlc, videoName.c_str());

    if (!media)
    {
      if(libvlc_errmsg()!=0)InvalidInputMessage("Failed to create new vlc media from:"+videoName+". Libvlc error message: "+std::string(libvlc_errmsg()));
      else InvalidInputMessage("Failed to create new vlc media from:"+videoName+"; there is no vlc error message to provide"); 
    }
    
    libvlc_media_player_t *mediaplayer =
        libvlc_media_player_new_from_media(media);

    libvlc_media_release(media);

    if (!mediaplayer)
      InvalidInputMessage("Failed to create VLC media player");

    // this was weirdly complicated
    SDL_PropertiesID props = SDL_GetWindowProperties(window);

    Sint64 x11Window =
        SDL_GetNumberProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);

    if (x11Window == 0)
      InvalidInputMessage("Failed to fetch X11 Window ID");

    libvlc_media_player_set_xwindow(mediaplayer,
                                    static_cast<uint32_t>(x11Window));

    restartSong();

    if (libvlc_media_player_play(mediaplayer) == -1)
      InvalidInputMessage("Failed to start VLC playback");

    while (!libvlc_media_player_is_playing(mediaplayer))
      SDL_Delay(10); // Player has not started to play yet so we have to wait
                     // until it is ready to seek

    libvlc_time_t positionMs =
        static_cast<libvlc_time_t>(result.offsetInSeconds * 1000.0);
    libvlc_media_player_set_time(mediaplayer, positionMs);

    while (running &&libvlc_media_player_get_state(mediaplayer) != libvlc_Ended&&GetCurrentPlayingInfo()==current) {
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT)
          running = false;
      
      }

      SDL_Delay(10);
    }

  libvlc_media_player_stop(mediaplayer);
  libvlc_media_player_release(mediaplayer);
  libvlc_release(vlc);
  }


  SDL_DestroyWindow(window);
  SDL_Quit();
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