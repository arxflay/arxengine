/*
 * @author Alexej Fedorenko
 */

#ifndef ARX_SOUNDPLAYER_H
#define ARX_SOUNDPLAYER_H
#include "arxengine/ArxDefines.h"
#include <memory>
#include <string_view>
ARX_NAMESPACE_BEGIN

class Sound;
class SoundContext;

class ARX_EXPORTS SoundPlayer final
{
public:
    enum PlayMode
    {
        Async, //runs in openal thread 
        Sync //runs in current thread
    };
    //for now accept only default device 
    SoundPlayer();
    void SetVolumeInPercentage(float value);
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
