#include "sourceAL.h"

SourceAL::SourceAL(){
    alGenSources(1, &sourceID);
    alSourcef(sourceID, AL_GAIN, 1);
    alSourcef(sourceID, AL_PITCH, 1);
    alSource3f(sourceID, AL_POSITION, 0, 0, 0);
}

SourceAL::~SourceAL(){
    stop();
    alDeleteSources(1, &sourceID);
}

void SourceAL::play(ALint buffer){
    stop();
    alSourcei(sourceID, AL_BUFFER, buffer);
    continuePlaying();
}

void SourceAL::pause(){
    alSourcePause(sourceID);
}

void SourceAL::continuePlaying(){
    alSourcePlay(sourceID);
}

void SourceAL::stop(){
    alSourceStop(sourceID);
}

void SourceAL::setVolume(ALfloat volume){
    alSourcef(sourceID, AL_GAIN, volume);
}

void SourceAL::setVelocity(ALfloat x, ALfloat y, ALfloat z){
    alSource3f(sourceID, AL_VELOCITY, x, y, z);
}

void SourceAL::setPitch(ALfloat pitch){
    alSourcef(sourceID, AL_PITCH, pitch);
}

void SourceAL::setPosition(ALfloat x, ALfloat y, ALfloat z){
    alSource3f(sourceID, AL_POSITION, x, y, z);
}

void SourceAL::setLooping(ALboolean loop){
    alSourcei(sourceID, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
}

ALboolean SourceAL::isPlaying(){
    ALint current_State;
    alGetSourcei(sourceID, AL_SOURCE_STATE, &current_State);
    return  current_State == AL_PLAYING;
}
