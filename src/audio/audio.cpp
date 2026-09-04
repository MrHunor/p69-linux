/*
 * Copyright (C) 2026 MrHunor
 * LICENSE:GNU General Public License v3 (GPLv3)
 */
#include "../utils/utils.h"
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <limits>
#include <portaudio.h>
#include <sndfile.h>
#include <string>
#include <vector>

#include "../audio/audio.h"
#include <cmath>
#include <cstddef>
#include <format>
#include <kissfft/kiss_fft.h>
#include <limits>
#include <vector>

namespace fs = std::filesystem;

std::vector<float> loadWavMonoToVector(const std::string &filename) {
  SF_INFO info{};
  SNDFILE *file = sf_open(filename.c_str(), SFM_READ, &info);
  if (!file)
    InvalidInputMessage("Failed to open wav");
  if (info.channels != 1)
    InvalidInputMessage("WAV MUST be mono!");
  std::vector<float> audio(info.frames);
  const sf_count_t framesRead = sf_readf_float(file, audio.data(), info.frames);
  sf_close(file);
  if (framesRead != info.frames)
    InvalidInputMessage("Count of read frames does not match count of actually "
                        "existing frames");
  return audio;
}

std::string ConvertToMono(const std::string &input, const std::string &output,
                          int sampleRate) {
  if (input == output) {
    std::string temp = output + ".tmp.wav";

    std::string cmd =
        std::format("ffmpeg -y -i '{}' -ac 1 -ar {} '{}' && mv '{}' '{}'",
                    input, sampleRate, temp, temp, output);

    return executeCommand(cmd);
  }

  std::string cmd = std::format("ffmpeg -y -i '{}' -ac 1 -ar {} '{}'", input,
                                sampleRate, output);

  return executeCommand(cmd);
}

std::string RemoveAudio(const std::string &input, const std::string &output) {
  if (input == output) {
    std::string temp = output + ".tmp.mp4";

    std::string cmd =
        std::format("ffmpeg -y -i '{}' -c:v copy -an '{}' && mv '{}' '{}'",
                    input, temp, temp, output);

    return executeCommand(cmd);
  }

  std::string cmd =
      std::format("ffmpeg -y -i '{}' -c:v copy -an '{}'", input, output);

  return executeCommand(cmd);
}

int getNextPowerOfTwo(int n) {
  int p = 1;
  while (p < n) {
    p <<= 1; // weird bitshifty way of next power of two
  }
  return p;
}

// god knows how fft works
// a score of >0.5 can be considerd a match, anything under that is questionable
fftMatchResult findMatch(const std::vector<float> &origin,
                         const std::vector<float> &clip, double sampleRate) {
  // definitv sizes
  const int originSize = origin.size();
  const int clipSize = clip.size();
  const int convultionSize =
      originSize + clipSize - 1; // god knows waht convultion means
  const int fftSize = getNextPowerOfTwo(convultionSize);

  // allocate fft buffers
  std::vector<kiss_fft_cpx> originFFT(fftSize);
  std::vector<kiss_fft_cpx> clipFFT(fftSize);

  std::vector<kiss_fft_cpx> product(fftSize);
  std::vector<kiss_fft_cpx> correlation(fftSize);

  // putting the origin buffer into the fft buffer
  for (int i = 0; i < originSize; i++) {
    originFFT[i].r = origin[i];
    originFFT[i].i =
        0.0f; // im not even gonna predent as to know what this is for
  }

  // reversing the clip; logic: convultion reversed cliup = cross-correlation
  for (int i = 0; i < clipSize; i++) {
    clipFFT[i].r = clip[clipSize - 1 - i];
    clipFFT[i].i = 0.0f;
  }

  // FFT config
  kiss_fft_cfg forward = kiss_fft_alloc(fftSize, 0, nullptr, nullptr);
  kiss_fft_cfg inverse = kiss_fft_alloc(fftSize, 1, nullptr, nullptr);
  if (!forward || !inverse) {
    if (forward)
      kiss_fft_free(forward);

    if (inverse)
      kiss_fft_free(inverse);

    InvalidInputMessage("Failed to allocate for Kiss fft");
  }

  // convert signals from time to frequency domain
  kiss_fft(forward, originFFT.data(), originFFT.data());
  kiss_fft(forward, clipFFT.data(), clipFFT.data());

  // Multiplication Time! (Only Gaus understands this Magic)
  for (int i = 0; i < fftSize; i++) {
    const float ar = originFFT[i].r;
    const float ai = originFFT[i].i;

    const float br = clipFFT[i].r;
    const float bi = clipFFT[i].i;

    product[i].r = ar * br - ai * bi; //(ar + i*ai) * (br + i*bi)
    product[i].i = ar * bi + ai * br;
  }

  // back to the time domain!
  kiss_fft(inverse, product.data(), correlation.data());

  // nromalise the inverse FFT
  const double fftScale = 1.0 / fftSize;

  // Calculate statistcs (Zero-mean), yeah idfk

  double clipSum = 0.0;
  double clipSumSq = 0.0;

  for (float sample : clip) {
    clipSum = clipSum + sample;
    clipSumSq = clipSumSq + static_cast<double>(sample) * sample;
  }

  const double clipMean = clipSum / static_cast<double>(clipSize);

  const double clipEnergy = clipSumSq - (clipSum * clipSum) / clipSize;

  if (clipEnergy <= 1e-12) // this is a safetly value to protect against weird
                           // float calculations
  {
    kiss_fft_free(forward);
    kiss_fft_free(inverse);

    InvalidInputMessage("Clip has no usable singal variablity");
  }

  // prefix sums for the BIG audio
  std::vector<double> prefixSum(originSize + 1, 0.0);
  std::vector<double> prefixSumSq(originSize + 1, 0.0);

  for (size_t i = 0; i < originSize; ++i) {
    prefixSum[i + 1] = prefixSum[i] + origin[i];

    prefixSumSq[i + 1] =
        prefixSumSq[i] + static_cast<double>(origin[i]) * origin[i];
  }
  // search for best overlap
  double bestScore = -std::numeric_limits<double>::infinity();
  int bestOffset = 0;
  const int numberOfOffsets = originSize - clipSize + 1;

  for (size_t offset = 0; offset < numberOfOffsets; ++offset) {
    const size_t start = offset;
    const size_t end = offset + clipSize;

    const double windowSum = prefixSum[end] - prefixSum[start];

    const double windowSumSq = prefixSumSq[end] - prefixSumSq[start];

    const double windowEnergy =
        windowSumSq - (windowSum * windowSum) / clipSize;

    if (windowEnergy <= 1e-12)
      continue;

    const size_t correlationIndex = offset + clipSize - 1;

    const double rawCorrelation = correlation[correlationIndex].r * fftScale;

    const double numerator = rawCorrelation - (windowSum * clipSum) / clipSize;

    const double denominator = std::sqrt(windowEnergy * clipEnergy);

    const double score = numerator / denominator;

    if (score > bestScore) {
      bestScore = score;
      bestOffset = offset;
    }
  }
  // Cleanup
  kiss_fft_free(forward);
  kiss_fft_free(inverse);
  return {bestOffset, static_cast<double>(bestOffset) / sampleRate, bestScore};
}

void CaptureAudio(double durationSeconds, const std::string &outputFile) {

  std::string command = std::format(
      "ffmpeg -y -f pulse -i \"$(pactl get-default-sink).monitor\" -t {} {}",
      durationSeconds, outputFile);
  executeCommand(command);
}
