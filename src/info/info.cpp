/*
 * Copyright (C) 2026 MrHunor
 * LICENSE:GNU General Public License v3 (GPLv3)
 */


#include "../audio/audio.h"
#include "../utils/utils.h"
#include "../utils/defs.h"
#include "SDL3/SDL.h"
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


float calculateDrawLineLength(int pos, int length, SDL_FRect time,
                              SDL_FRect len) {
 float linemax = len.x - (time.x + time.w);
    float progress = static_cast<float>(pos) / static_cast<float>(length);
    float result = progress * linemax;



    return result;
}



void runInfoLoop(stateClass &state) {
  // define vars
  // yes this is ugly and slow
  bool running = true;
  SDL_Event event;

  while (running) {
    const std::string title = removeNewLineAndReturnCharacters(
        executeCommand("playerctl metadata xesam:title"));
    const std::string artist = removeNewLineAndReturnCharacters(
        executeCommand("playerctl metadata xesam:artist"));
    const std::string album = removeNewLineAndReturnCharacters(
        executeCommand("playerctl metadata xesam:album"));
    const std::string length =
        SecToMinAndSec(stoi(removeNewLineAndReturnCharacters(
                           executeCommand("playerctl metadata mpris:length"))) /
                       1000000);
    const std::string trackNumber = removeNewLineAndReturnCharacters(
        executeCommand("playerctl metadata xesam:trackNumber"));
    const std::string artUrl = removeNewLineAndReturnCharacters(
        executeCommand("playerctl metadata mpris:artUrl"));
    SDL_Color white = {255, 255, 255, 255};
    float text_w;
    float text_h;

    state.out("Queried the following:\ntitle:" + title + "\nartist:" + artist +
                  "\nalbum:" + album +
                  "\nlength(in s):" + std::to_string(stoi(length) / 1000000) +
                  "\ntrackNumber:" + trackNumber + "\nartUrl" + artUrl,
              4);

    state.out("Setting up SDL&TTF..", 4);
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER, "x11");
    if (!SDL_Init(SDL_INIT_VIDEO))
      InvalidInputMessage("Failed to initialise SDL");
    if (!TTF_Init())
      InvalidInputMessage("Failed to inilised SDL-TTF");

    state.resX = state.resYRequested * 16 / 9;
    state.resY = state.resYRequested;

    SDL_Window *window = SDL_CreateWindow("P69", state.resX, state.resY, 0);
    if (!window)
      InvalidInputMessage("Failed to initalise window");

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    state.out("Finished.", 4);

    // album art
    state.out("Creating image texture & Rect...", 4);
    state.out("Curl output:\n" +
                  executeCommand(std::format("curl --output \"{}\" {}",
                                             album + ".png", artUrl)),
              4);
    scaleImage(album + ".png", album + std::to_string(state.resY) + ".png",
               state.resY / 2);
    SDL_Texture *image = IMG_LoadTexture(
        renderer,
        std::string(album + std::to_string(state.resY) + ".png").c_str());
    if (!image)
      InvalidInputMessage("Failed to initlise image:" + album +
                          std::to_string(state.resY) + ".png");
    SDL_FRect image_rect = {
        static_cast<float>(state.resX / 10), static_cast<float>(state.resY / 4),
        static_cast<float>(image->w), static_cast<float>(image->h)};
    state.out("Finished.", 4);

    // Texts
    state.out("Creating text textures & Rects...", 4);
    // font
    TTF_Font *font003 =
        TTF_OpenFont("/usr/share/fonts/TTF/Hack-Bold.ttf", state.resY * 0.03);
    TTF_Font *font002 =
        TTF_OpenFont("/usr/share/fonts/TTF/Hack-Bold.ttf", state.resY * 0.02);
    TTF_Font *font001 =
        TTF_OpenFont("/usr/share/fonts/TTF/Hack-Bold.ttf", state.resY * 0.01);

    SDL_Surface *titleSurface =
        TTF_RenderText_Blended(font003, title.c_str(), 0, white);
    SDL_Surface *artistSurface =
        TTF_RenderText_Blended(font002, artist.c_str(), 0, white);
    SDL_Surface *albumSurface =
        TTF_RenderText_Blended(font002, album.c_str(), 0, white);
    SDL_Surface *lengthSurface =
        TTF_RenderText_Blended(font002, length.c_str(), 0, white);
    SDL_Surface *trackNumberSurface =
        TTF_RenderText_Blended(font003, trackNumber.c_str(), 0, white);
    SDL_Surface *timestampSurface;

    // textures
    SDL_Texture *titleTexture =
        SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Texture *artistTexture =
        SDL_CreateTextureFromSurface(renderer, artistSurface);
    SDL_Texture *albumTexture =
        SDL_CreateTextureFromSurface(renderer, albumSurface);
    SDL_Texture *lengthTexture =
        SDL_CreateTextureFromSurface(renderer, lengthSurface);
    SDL_Texture *trackNumberTexture =
        SDL_CreateTextureFromSurface(renderer, trackNumberSurface);
    SDL_Texture *timestampTexture;

    // free surfaces, they wont be used anymore
    SDL_DestroySurface(titleSurface);
    SDL_DestroySurface(artistSurface);
    SDL_DestroySurface(albumSurface);
    SDL_DestroySurface(lengthSurface);
    SDL_DestroySurface(trackNumberSurface);

    // Create rects for the texts
    SDL_GetTextureSize(titleTexture, &text_w, &text_h);
    SDL_FRect titleRect = {static_cast<float>(state.resX / 2.5),
                           static_cast<float>(state.resY / 4), text_w, text_h};
    SDL_GetTextureSize(artistTexture, &text_w, &text_h);
    SDL_FRect artistRect = {static_cast<float>(state.resX / 2.5),
                            static_cast<float>(state.resY / 3.5), text_w,
                            text_h};
    SDL_GetTextureSize(albumTexture, &text_w, &text_h);
    SDL_FRect albumTextureRect = {static_cast<float>(state.resX / 2.5),
                                  static_cast<float>(state.resY / 3), text_w,
                                  text_h};
    SDL_GetTextureSize(lengthTexture, &text_w, &text_h);
    SDL_FRect lengthRect = {static_cast<float>(state.resX / 1.5),
                            static_cast<float>(state.resY / 1.5), text_w,
                            text_h};
    SDL_GetTextureSize(trackNumberTexture, &text_w, &text_h);
    SDL_FRect trackNumberRect = {static_cast<float>(state.resX / 1.5),
                                 static_cast<float>(state.resY / 4), text_w,
                                 text_h};
    SDL_FRect timestampRect{static_cast<float>(state.resX / 2.5),
                            static_cast<float>(state.resY / 1.5), 0, 0};
    state.out("Finished.", 4);

    while (title == removeNewLineAndReturnCharacters(
                        executeCommand("playerctl metadata xesam:title")) &&
           running) {
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT)
          running = false;
      }
      timestampSurface = TTF_RenderText_Blended(
          font002,
          SecToMinAndSec(stoi(removeNewLineAndReturnCharacters(
                             executeCommand("playerctl position"))))
              .c_str(),
          0, white);
      timestampTexture =
          SDL_CreateTextureFromSurface(renderer, timestampSurface);
      SDL_GetTextureSize(timestampTexture, &text_w, &text_h);
      timestampRect.w = text_w;
      timestampRect.h = text_h;

      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

      SDL_RenderLine(renderer, timestampRect.x + timestampRect.w + 5,
                     timestampRect.y + (timestampRect.h / 2),
                     timestampRect.x + timestampRect.w + 5 +calculateDrawLineLength(
                         stoi(removeNewLineAndReturnCharacters(
                             executeCommand("playerctl position"))),
                         stoi(removeNewLineAndReturnCharacters(executeCommand(
                             "playerctl metadata mpris:length")))/1000000,
                         timestampRect, lengthRect),
                     timestampRect.y + (timestampRect.h / 2));
      // render textures
      state.out("Rendering...", 4);
      SDL_RenderTexture(renderer, titleTexture, nullptr, &titleRect);
      SDL_RenderTexture(renderer, artistTexture, nullptr, &artistRect);
      SDL_RenderTexture(renderer, albumTexture, nullptr, &albumTextureRect);
      SDL_RenderTexture(renderer, lengthTexture, nullptr, &lengthRect);
      SDL_RenderTexture(renderer, trackNumberTexture, nullptr,
                        &trackNumberRect);
      SDL_RenderTexture(renderer, timestampTexture, 0, &timestampRect);

      // render Image
      SDL_RenderTexture(renderer, image, NULL, &image_rect);
      state.out("Finished", 4);
      SDL_RenderPresent(renderer);
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderClear(renderer);

      SDL_Delay(100);
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
  state.out("Recived exit signal, this is the end of the Loop.", 4);
}