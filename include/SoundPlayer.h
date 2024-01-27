#pragma once
#include "ArxDefines.h"
ARX_NAMESPACE_BEGIN

class Sound;

class SoundPlayer final
{
public:
    enum PlayMode
    {
        Async, //runs in openal thread 
        Sync //runs in current thread
    };

    SoundPlayer();
    void LoadSound(const Sound &sound);
    void Play(PlayMode mode, bool loop = false);
    bool IsPlaying();
    void Stop();
    ~SoundPlayer();
private:
    
    unsigned int m_buffer;
    unsigned int m_source;
};

ARX_NAMESPACE_END
