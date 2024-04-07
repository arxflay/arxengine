#include "arxengine/media/SoundPlayer.h"
#include "arxengine/media/Sound.h"
#include <AL/al.h>
#include "arxengine/logging/Logger.h"
#include <AL/alc.h>
#include <memory>
#include "arxengine/GameApp.h"
#include "arxengine/internal/al/SoundDevice.h"
#include <iostream>
ARX_NAMESPACE_BEGIN

SoundPlayer::SoundPlayer()
{
    m_soundContext = &GameApp::GetGlobalApp()->GetDefaultSoundDeviceContext();
    m_soundContext->Bind();

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
    m_soundContext->Bind();

    ALenum openalFormat = GetOpenALFormat(sound);
    if (openalFormat == AL_INVALID_VALUE)
    {
        GLOG->Error("SoundPlayer::LoadSound() => invalid openal format");
        return;
    }
    Stop();
    alSourcei(m_source, AL_BUFFER, 0); //unbind buffer
    alBufferData(m_buffer, GetOpenALFormat(sound), sound.GetData().data(), 
            static_cast<ALsizei>(sound.GetData().size()), static_cast<ALsizei>(sound.GetSampleFrequency()));
    alSourcei(m_source, AL_BUFFER, static_cast<ALint>(m_buffer));
}

void SoundPlayer::SetVolumeInPercentage(float value)
{
    alSourcef(m_source, AL_GAIN, std::max(value / 100.0f, 0.0f));
}

void SoundPlayer::Play(PlayMode mode, bool loop)
{
    m_soundContext->Bind();

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
    m_soundContext->Bind();
    alSourceStop(m_source);
}

SoundPlayer::~SoundPlayer()
{
    m_soundContext->Bind();
    alDeleteSources(1, &m_source);
    alDeleteBuffers(1, &m_buffer);
}

ARX_NAMESPACE_END
