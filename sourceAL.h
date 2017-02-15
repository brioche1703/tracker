#ifndef SOURCEAL_H
#define SOURCEAL_H

#include <string>
#include "AL/al.h"


class SourceAL {

    public:
        /*  Functions  */
        SourceAL();
        ~SourceAL();

        void play(ALint buffer);
        void pause();
        void continuePlaying();
        void stop();

        void setVolume(ALfloat volume);
        void setVelocity(ALfloat x, ALfloat y, ALfloat z);
        void setPitch(ALfloat pitch);
        void setPosition(ALfloat x, ALfloat y, ALfloat z);
        void setLooping(ALboolean loop);

        ALboolean isPlaying();

        inline ALenum getSoureID() const{return sourceID;}

    private:
        ALuint sourceID;

};

#endif // SOURCEAL_H
