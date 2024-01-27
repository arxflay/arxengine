#include "SoundPlayer.h"
#include "Sound.h"
#include <AL/al.h>
#include "logging/Logger.h"
#include <AL/alc.h>
ARX_NAMESPACE_BEGIN

SoundPlayer::SoundPlayer()
{
    //app must be initialized
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
    alSourceStop(m_source);
}

SoundPlayer::~SoundPlayer()
{
    alDeleteSources(1, &m_source);
    alDeleteBuffers(1, &m_buffer);
}

ARX_NAMESPACE_END
