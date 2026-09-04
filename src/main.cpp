/*
 * Copyright (C) 2026 MrHunor
 * LICENSE:GNU General Public License v3 (GPLv3)
 */
#include "CLI/CLI.hpp"
#include "utils/utils.h"
#include "vid/vid.h"
#include <CLI/CLI.hpp>
int main(int argc, char *argv[]) {
  stateClass state;
  state.out("Starting", 4);

  // CLI config
  CLI::App app{"P69(-linux):A Portable Music Visualiser in the form of "
               "Musicvideos and (tbd) Lyrics. COPYRIGHT 2026 MrHunor, GPLv3"};
  auto *media_group =
      app.add_option_group("Media Visulasation Type (Video or Lycris)");
  app.set_help_all_flag("--help-all", "Expand and show all subcommand options");
  app.add_flag("-v,--verbose", state.verbose,
               "Enable verbose output (Can be stacked up to 4)");
  app.add_option("-t,--time,--capture-time", state.CaptureTime,
                 "Modify the capture time int seconds, default:5s");
  auto *res =
      app.add_option("-r, --resolution", state.resYRequested,
                     "Try to use AT LEAST this resoltion. Possible Resoltions "
                     "can be:720,1080,1440,1800,2160. Default:720");
  res->check(CLI::IsMember({720, 1080, 1440, 1800, 2160}));
  auto *video = media_group->add_flag("-V,--video", "Show Musicvideo");
  auto *lyrics = media_group->add_flag("-L,--lyrics", "Show Lyrics");
  media_group->require_option(1);

  app.callback([&]() {
    if (*video)
      runVideoLoop(state);
    if (*lyrics)
      InvalidInputMessage("Sorry but Lyrics mode is yet to be implemented");
  });

  CLI11_PARSE(app, argc, argv);
  return 0;
}