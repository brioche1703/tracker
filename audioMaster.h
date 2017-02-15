#ifndef AUDIOMASTER_H
#define AUDIOMASTER_H


#include <stdio.h>
#include <string>
#include <iostream>
#include <AL/al.h>
#include <AL/alc.h>
#include <vector>

/*
 * Struct that holds the RIFF data of the Wave file.
 * The RIFF data is the meta data information that holds,
 * the ID, size and format of the wave file
 */
struct RIFF_Header {
  char chunkID[4];
  int chunkSize;//size not including chunkSize or chunkID
  char format[4];
};

/*
 * Struct to hold fmt subchunk data for WAVE files.
 */
struct WAVE_Format {
  char subChunkID[4];
  int subChunkSize;
  short audioFormat;
  short numChannels;
  int sampleRate;
  int byteRate;
  short blockAlign;
  short bitsPerSample;
};

/*
* Struct to hold the data of the wave file
*/
struct WAVE_Data {
  char subChunkID[4]; //should contain the word data
  int subChunk2Size; //Stores the size of the data block
};

class AudioMaster {

    public:
        /*  Functions  */
        AudioMaster();
        ~AudioMaster();

        void init();
        ALuint loadSound(std::string file);
        void setListenerDate(ALfloat x, ALfloat y, ALfloat z);
        void cleanUp();

        static bool loadWavFile(const std::string filename, ALuint* buffer, ALsizei* size, ALsizei* frequency, ALenum* format);
    private:
        std::vector<ALuint> buffers;;
        ALCdevice* device;
        ALCcontext* context;

};
#endif // AUDIOMASTER_H
