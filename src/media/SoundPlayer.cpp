#include "media/SoundPlayer.h"
#include "media/Sound.h"
#include <AL/al.h>
#include "logging/Logger.h"
#include <AL/alc.h>
#include <memory>
#include "GameApp.h"

ARX_NAMESPACE_BEGIN

namespace internal
{
    class SoundDevice
    {
    public:
        SoundDevice(std::string_view deviceName)
            : m_device(nullptr, alcCloseDevice)
        {
            m_device.reset(alcOpenDevice(deviceName.data()));
            if (!m_device)
                throw ArxException(ArxException::ErrorCode::FailedToInitializeOpenAL, "Failed to create device");
        }
        
        ~SoundDevice()
        {
            alcCloseDevice(m_device.get());
        }

    class SoundContext
    {
    public:
        SoundContext(SoundDevice &device)
            : m_context(nullptr, alcDestroyContext)
        {
            m_context.reset(alcCreateContext(device.m_device.get(), nullptr));
            if(!m_context)
                throw ArxException(ArxException::ErrorCode::FailedToInitializeOpenAL, "Failed to create openal context");
        }
        
        void SetAsCurrentContext()
        {
            alcMakeContextCurrent(m_context.get());
        }

        ~SoundContext()
        {
            if (alcGetCurrentContext() == m_context.get())
                alcMakeContextCurrent(nullptr);
        }
        private:
            std::unique_ptr<ALCcontext, void(*)(ALCcontext*)>  m_context;
    };

    SoundContext CreateContext()
    {
        return SoundContext(*this);
    }

    private:
        std::unique_ptr<ALCdevice, char(*)(ALCdevice*)>  m_device;
    };

    class CurrentALCcontextGuard
    {
    public:
        CurrentALCcontextGuard()
            : m_currentAlcContext(alcGetCurrentContext())
        {
        }

        ~CurrentALCcontextGuard()
        {
            alcMakeContextCurrent(m_currentAlcContext);
        }

        ALCcontext *m_currentAlcContext;
    };
}

class SoundDeviceContextPair
{
public:
    SoundDeviceContextPair(std::string_view deviceName)
        : m_device(deviceName), m_context(m_device)
    {
    }
    
    void MakeContextCurrent()
    {
        m_context.SetAsCurrentContext();
    }
private:
    internal::SoundDevice m_device;
    internal::SoundDevice::SoundContext m_context;
};

void DestroySoundDeviceContextPair(SoundDeviceContextPair *pair)
{
    delete pair;
}

SoundDeviceContextPair *CreateSoundDeviceContextPair(std::string_view deviceName)
{
    return std::make_unique<SoundDeviceContextPair>(deviceName).release();
}

SoundPlayer::SoundPlayer(SoundDeviceContextPair *pair)
{
    if(pair == nullptr)
        m_pair = &GameApp::GetGlobalApp()->GetSoundDeviceContextPair();
    else
        m_pair = pair;

    internal::CurrentALCcontextGuard guard;
    m_pair->MakeContextCurrent();

    alGenSources(1, &m_source);
    alGenBuffers(1, &m_buffer);
}

ALenum GetOpenALFormat(const Sound &sound)
{
    switch (sound.GetBitDepth())
    {
        case 8:
            return (sound.GetSoundReproductionType() == Sound::SoundReproductionType::Mono) ? AL_FORMAT_MONO8 : AL_FORMAT_STEREO8;
        case 16:
            return (sound.GetSoundReproductionType() == Sound::SoundReproductionType::Mono) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
        default:
            return AL_INVALID_VALUE;
    }
}

void SoundPlayer::LoadSound(const Sound &sound)
{
    internal::CurrentALCcontextGuard guard;
    m_pair->MakeContextCurrent();

    ALenum openalFormat = GetOpenALFormat(sound);
    if (openalFormat == AL_INVALID_VALUE)
    {
        GLOG->Error("SoundPlayer::LoadSound() => invalid format");
        return;
    }
    alBufferData(m_buffer, GetOpenALFormat(sound), sound.GetData().data(), 
            static_cast<ALsizei>(sound.GetData().size()), static_cast<ALsizei>(sound.GetSampleFrequency()));
    alSourcei(m_source, AL_BUFFER, static_cast<ALint>(m_buffer));
}

void SoundPlayer::Play(PlayMode mode, bool loop)
{
    internal::CurrentALCcontextGuard guard;
    m_pair->MakeContextCurrent();

    alSourceStop(m_source);
    alSourcei(m_source, AL_LOOPING, static_cast<ALint>(loop));
    alSourcePlay(m_source);

    if (mode == PlayMode::Sync)
        while(IsPlaying());
}

bool SoundPlayer::IsPlaying()
{
    ALint state;
    alGetSourcei(m_source, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING; 
}

void SoundPlayer::Stop()
{
    internal::CurrentALCcontextGuard guard;
    m_pair->MakeContextCurrent();
    alSourceStop(m_source);
}

SoundPlayer::~SoundPlayer()
{
    internal::CurrentALCcontextGuard guard;
    m_pair->MakeContextCurrent();
    alDeleteSources(1, &m_source);
    alDeleteBuffers(1, &m_buffer);
}

ARX_NAMESPACE_END
