/*
 * Copyright (C) 2026 MrHunor
 * LICENSE:GNU General Public License v3 (GPLv3)
 */
#include "../utils/utils.h"
#include "SDL3/SDL.h"
#include "../audio/audio.h"
#include "vid.h"
#include "vlc/vlc.h"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <cstddef>
#include <string>
#include <unistd.h>
#include <vlc/libvlc.h>
#include <vlc/libvlc_media.h>
#include <vlc/libvlc_vlm.h>
std::string GetCurrentPlayingInfo()
{
return executeCommand(std::string("playerctl metadata --format '{{ artist }} - {{ title }}'"));
}

void runVideoLoop(stateClass& state)
{
    bool running = true;
    state.resX= state.resYRequested * 16 / 9;
    state.resY = state.resYRequested;

  while (running == true) {
    state.out("Quering current playing Info..", 4);
    const std::string current = GetCurrentPlayingInfo();
    state.out("Current Playing:" + current, 4);

    state.out("Downloading Video...", 4);
    std::string videoName = DownloadVideo(current,state.resYRequested);
    state.out("Downloaded video name:" + videoName, 4);
    
    const int trueRes = getVideoHeight(videoName);
    if(trueRes!=state.resYRequested)
    {
      state.out("Best possible resolution was:"+std::to_string(trueRes)+". Resizing...",4);
      state.resX= trueRes * 16 / 9;
      state.resY = trueRes;
      state.out(
    "SDL window size is now " +std::to_string(trueRes),4);
    }

    state.out("Attempting to restart song...", 4);
    restartSong();

    state.out("Capturing "+std::to_string(state.CaptureTime)+" seconds of audio into sample.wav", 4);
    CaptureAudio(state.CaptureTime, "sample.wav");

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
  
   SDL_Event event;
  SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "x11");

  if (!SDL_Init(SDL_INIT_VIDEO))InvalidInputMessage("Failed to initialise SDL: " +std::string(SDL_GetError()));

  SDL_Window *window = SDL_CreateWindow("P69", state.resX, state.resY, SDL_WINDOW_RESIZABLE);

  if (!window)InvalidInputMessage("Failed to create SDL window: " +std::string(SDL_GetError()));

 
    const std::string vlcverbosearg= "--verbose="+std::to_string(state.verbose);
    const char *args[] = {
    vlcverbosearg.c_str()

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

  SDL_DestroyWindow(window);
  SDL_Quit();
}


}


int getVideoHeight(const std::string& videoName)
{
  int y= stoi(executeCommand("ffprobe -v error -select_streams v:0 -show_entries stream=height -of csv=p=0  \""+videoName+"\""));
  return y;
}


std::string DownloadVideo(const std::string& videoName, int resoltuinH)
{
    int exitCode; 
    std::string query = videoName;
    query=removeNewLineAndReturnCharacters(query);

 std::string cmd =
    "yt-dlp -S \"vcodec:h264,res:"+std::to_string(resoltuinH)+"\" "
    "--print after_move:filepath "
    "\"ytsearch:" + query + "\"";
    
    std::string retval = executeCommand(cmd);
    return removeNewLineAndReturnCharacters(retval);
    
}

std::string microSecToMinAndSec(std::string str)
{
  int min=0;
  int sec=0;
  int num = stoi(str);
  num = num/1000000;
  while(num>59)
  {
    num=num-60;
    min++;
  }
  sec = num;
  return std::to_string(min)+":"+std::to_string(sec);
}


void runInfoLoop(stateClass& state)
{
  //define vars 
  //yes this is ugly and slow
  bool running=true;
  SDL_Event event;

  while(running)
  {
  const std::string title = removeNewLineAndReturnCharacters(executeCommand("playerctl metadata xesam:title"));
  const std::string artist = removeNewLineAndReturnCharacters(executeCommand("playerctl metadata xesam:artist"));
  const std::string album = removeNewLineAndReturnCharacters(executeCommand("playerctl metadata xesam:album"));
  const std::string length = microSecToMinAndSec(removeNewLineAndReturnCharacters(executeCommand("playerctl metadata mpris:length")));
  const std::string trackNumber = removeNewLineAndReturnCharacters(executeCommand("playerctl metadata xesam:trackNumber"));
  const std::string artUrl = removeNewLineAndReturnCharacters(executeCommand("playerctl metadata mpris:artUrl"));
  SDL_Color white = {255,255,255,255};
  float text_w;
  float text_h;

  state.out("Queried the following:\ntitle:"+title+"\nartist:"+artist+"\nalbum:"+album+"\nlength(in s):"+std::to_string(stoi(length)/1000000)+"\ntrackNumber:"+trackNumber+"\nartUrl"+artUrl,4);

  state.out("Setting up SDL&TTF..",4);
  SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "x11");
  if(!SDL_Init(SDL_INIT_VIDEO))InvalidInputMessage("Failed to initialise SDL");
  if(!TTF_Init())InvalidInputMessage("Failed to inilised SDL-TTF");

  state.resX= state.resYRequested * 16 / 9;
  state.resY = state.resYRequested;

  SDL_Window *window = SDL_CreateWindow("P69",state.resX, state.resY,0);
  if(!window)InvalidInputMessage("Failed to initalise window");

  SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
  state.out("Finished.",4);

  //album art
  state.out("Creating image texture & Rect...",4);
  state.out("Curl output:\n"+executeCommand(std::format("curl --output \"{}\" {}",album+".png",artUrl)),4);
  scaleImage(album+".png",album+std::to_string(state.resY)+".png",state.resY/2);
  SDL_Texture *image = IMG_LoadTexture(renderer, std::string(album+std::to_string(state.resY)+".png").c_str());
  if(!image)InvalidInputMessage("Failed to initlise image:"+album+std::to_string(state.resY)+".png");
  SDL_FRect image_rect = {static_cast<float>(state.resX/10),static_cast<float>(state.resY/4),static_cast<float>(image->w),static_cast<float>(image->h)};
  state.out("Finished.",4);

//Texts
state.out("Creating text textures & Rects...",4);
//font
TTF_Font* font003 = TTF_OpenFont("/usr/share/fonts/TTF/Hack-Bold.ttf",state.resY*0.03);
TTF_Font* font002 = TTF_OpenFont("/usr/share/fonts/TTF/Hack-Bold.ttf",state.resY*0.02);
TTF_Font* font001 = TTF_OpenFont("/usr/share/fonts/TTF/Hack-Bold.ttf",state.resY*0.01);
  
 SDL_Surface *titleSurface =TTF_RenderText_Blended(font003, title.c_str(), 0, white);
SDL_Surface *artistSurface =TTF_RenderText_Blended(font002, artist.c_str(), 0, white);
SDL_Surface *albumSurface =TTF_RenderText_Blended(font002, album.c_str(), 0, white);
SDL_Surface *lengthSurface =TTF_RenderText_Blended(font002, length.c_str(), 0, white);
SDL_Surface *trackNumberSurface =TTF_RenderText_Blended(font003, trackNumber.c_str(), 0, white);

//textures
SDL_Texture *titleTexture =SDL_CreateTextureFromSurface(renderer, titleSurface);
SDL_Texture *artistTexture =SDL_CreateTextureFromSurface(renderer, artistSurface);
SDL_Texture *albumTexture =SDL_CreateTextureFromSurface(renderer, albumSurface);
SDL_Texture *lengthTexture =SDL_CreateTextureFromSurface(renderer, lengthSurface);
SDL_Texture *trackNumberTexture =SDL_CreateTextureFromSurface(renderer, trackNumberSurface);

//free surfaces, they wont be used anymore
SDL_DestroySurface(titleSurface);
SDL_DestroySurface(artistSurface);
SDL_DestroySurface(albumSurface);
SDL_DestroySurface(lengthSurface);
SDL_DestroySurface(trackNumberSurface);

//Create rects for the texts
SDL_GetTextureSize(titleTexture,&text_w,&text_h);
SDL_FRect titleRect = { static_cast<float>(state.resX/2.5),static_cast<float>(state.resY/4),text_w,text_h};
SDL_GetTextureSize(artistTexture,&text_w,&text_h);
SDL_FRect artistRect = { static_cast<float>(state.resX/2.5),static_cast<float>(state.resY/3.5),text_w,text_h};
SDL_GetTextureSize(albumTexture,&text_w,&text_h);
SDL_FRect albumTextureRect = { static_cast<float>(state.resX/2.5),static_cast<float>(state.resY/3),text_w,text_h};
SDL_GetTextureSize(lengthTexture,&text_w,&text_h);
SDL_FRect lengthRect = { static_cast<float>(state.resX/1.5),static_cast<float>(state.resY/1.5),text_w,text_h};
SDL_GetTextureSize(trackNumberTexture,&text_w,&text_h);
SDL_FRect trackNumberRect = { static_cast<float>(state.resX/1.5),static_cast<float>(state.resY/4),text_w,text_h};
state.out("Finished.",4);

//render textures
state.out("Rendering...",4);
SDL_RenderTexture(renderer, titleTexture, nullptr, &titleRect);
SDL_RenderTexture(renderer, artistTexture, nullptr, &artistRect);
SDL_RenderTexture(renderer, albumTexture, nullptr, &albumTextureRect);
SDL_RenderTexture(renderer, lengthTexture, nullptr, &lengthRect);
SDL_RenderTexture(renderer, trackNumberTexture, nullptr, &trackNumberRect);

//render Image
SDL_RenderTexture(renderer,image,NULL,&image_rect) ;
state.out("Finished",4);
SDL_RenderPresent(renderer);
  
while(title == removeNewLineAndReturnCharacters(executeCommand("playerctl metadata xesam:title")))
{
  while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT)
          running = false;
      
      }

      SDL_Delay(10);

}
SDL_DestroyTexture(titleTexture);
SDL_DestroyTexture(artistTexture);
SDL_DestroyTexture(albumTexture);
SDL_DestroyTexture(lengthTexture);
SDL_DestroyTexture(trackNumberTexture);
SDL_DestroyTexture(image);

TTF_CloseFont(font001);
TTF_CloseFont(font002);
TTF_CloseFont(font003);

SDL_DestroyRenderer(renderer);
SDL_DestroyWindow(window);

TTF_Quit();
SDL_Quit();
  }
  state.out("Recived exit signal, this is the end of the Loop.",4);
}