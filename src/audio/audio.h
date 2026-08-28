#include <iostream>
#include <vector>
struct fftMatchResult
{
int offsetInSamples;
double offsetInSeconds;
//Normalised corss-corelation score with +1.0 being highest and -.0 being inverted and 0 being no
double Score;
};
std::string ConvertToMono(const std::string& input,const std::string& output,int sampleRate);
std::vector<float> loadWavMonoToVector(const std::string& filename);
fftMatchResult findMatch( const std::vector<float>& origin, const std::vector<float>& clip, double sampleRate);
void CaptureAudio(double durationSeconds, const std::string& outputFile);
void saveAudioVectorToWav(const std::string& filename,std::vector<float>& audio, int sampleRate, int channels /*Assumes default format as in CaptureAudio()*/);