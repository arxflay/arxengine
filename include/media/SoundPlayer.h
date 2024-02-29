#ifndef ARX_SOUNDPLAYER_H
#define ARX_SOUNDPLAYER_H
#include "ArxDefines.h"
#include <memory>
#include <string_view>
ARX_NAMESPACE_BEGIN

class Sound;
class SoundContext;

class SoundPlayer final
{
public:
    enum PlayMode
    {
        Async, //runs in openal thread 
        Sync //runs in current thread
    };
    //for now accept only default device 
    SoundPlayer();
    void LoadSound(const Sound &sound);
    void Play(PlayMode mode, bool loop = false);
    bool IsPlaying();
    void Stop();
    ~SoundPlayer();
private:
    SoundContext *m_soundContext;
    unsigned int m_buffer;
    unsigned int m_source;
};

ARX_NAMESPACE_END

#endif
