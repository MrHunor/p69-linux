#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <portaudio.h>
#include "../utils/utils.h"
#include <sndfile.h>

void saveAudioVectorToWav(std::string& filename,std::vector<float>& audio, int sampleRate, int channels /*Assumes default format as in CaptureAudio()*/)
{
SF_INFO info{};
info.samplerate= sampleRate;
info.channels = channels;
info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

SNDFILE* file = sf_open("recording.wav", SFM_WRITE, &info);
if(!file)InvalidInputMessage("Failed to write to audio file:"+filename);
sf_writef_float(file,audio.data(), audio.size() / channels);
sf_close(file);
}

//ai assisted
bool CaptureAudio(double durationSeconds, const std::string& outputFile) {
   constexpr int sampleRate = 48000;
   constexpr int channels = 1;
   constexpr int framesPerBuffer = 256;

   if(Pa_Initialize()!=paNoError) InvalidInputMessage("Failed to Init portaudio");
   
   PaStream* stream = nullptr;
   
   PaError err = Pa_OpenDefaultStream(
    &stream,
    channels,
    0,
    paFloat32,
    sampleRate,
    framesPerBuffer,
    nullptr,
    nullptr
        );

    std::vector<float> buffer(framesPerBuffer*channels);
    std::vector<float> audio;
    audio.reserve(durationSeconds*sampleRate);
    
    while(audio.size()<durationSeconds*sampleRate)
    {
        Pa_ReadStream(stream,buffer.data(),framesPerBuffer);
        audio.insert(audio.end(),buffer.begin(),buffer.end());
    }
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();

  return false;
}