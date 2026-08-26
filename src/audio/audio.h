#include <iostream>
#include <vector>
void CaptureAudio(double durationSeconds, const std::string& outputFile);
void saveAudioVectorToWav(const std::string& filename,std::vector<float>& audio, int sampleRate, int channels /*Assumes default format as in CaptureAudio()*/);