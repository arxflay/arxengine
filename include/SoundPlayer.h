#pragma once
#include "ArxDefines.h"
#include <memory>
#include <string_view>
ARX_NAMESPACE_BEGIN

class Sound;
class SoundDeviceContextPair;

/*user is reponsible for cleanup*/
SoundDeviceContextPair *CreateSoundDeviceContextPair(std::string_view deviceName); //throws on failure
void DestroySoundDeviceContextPair(SoundDeviceContextPair *pair); //throws on failure

class SoundPlayer final
{
public:
    enum PlayMode
    {
        Async, //runs in openal thread 
        Sync //runs in current thread
    };
    
    SoundPlayer(SoundDeviceContextPair *SoundDeviceContextPair = nullptr);
    void LoadSound(const Sound &sound);
    void Play(PlayMode mode, bool loop = false);
    bool IsPlaying();
    void Stop();
    ~SoundPlayer();
private:
    SoundDeviceContextPair *m_pair;
    unsigned int m_buffer;
    unsigned int m_source;
};

ARX_NAMESPACE_END
