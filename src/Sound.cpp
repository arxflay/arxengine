#include "Sound.h"

#include "internal/WavLoader.h"
#include <AL/al.h>
ARX_NAMESPACE_BEGIN

Sound::SoundReproductionType Sound::GetSoundReproductionType() const { return m_soundReproductionType; }
uint32_t Sound::GetSampleFrequency() const { return m_sampleFreq; }
uint16_t Sound::GetBitDepth() const { return m_bitDepth; }
const std::vector<uint8_t> &Sound::GetData() const { return m_soundData; }

bool Sound::IsInvalid() const
{
    return m_soundData.empty() || m_bitDepth == 0 || m_sampleFreq == 0;
}

/*static*/ Sound Sound::LoadFromFile(std::string_view filename, Format format)
{
    (void)format; //reserved for future
    
    Sound sound;
    WavData wavData; 
    if(WavLoader::LoadWAVFile(filename, wavData) != WavLoader::WavLoadCode::NoError)
        return {};

    sound.m_soundReproductionType = (wavData.formatData.monoStereoFlag == (uint16_t)Sound::SoundReproductionType::Mono) ? SoundReproductionType::Mono : SoundReproductionType::Stereo;
    sound.m_sampleFreq = wavData.formatData.sampleFreq;
    sound.m_bitDepth = wavData.formatData.bitsPerSample;
    sound.m_soundData = std::move(wavData.soundData);
    
    return sound;
}

/*static*/ Sound Sound::LoadFromBinary(const std::vector<uint8_t> &data, Format format)
{
    (void)format; //reserved for future

    Sound sound;
    WavData wavData; 
    if(WavLoader::LoadWAVBinary(data, wavData) != WavLoader::WavLoadCode::NoError)
        return {};

    sound.m_soundReproductionType = (wavData.formatData.monoStereoFlag == (uint16_t)Sound::SoundReproductionType::Mono) ? SoundReproductionType::Mono : SoundReproductionType::Stereo;
    sound.m_sampleFreq = wavData.formatData.sampleFreq;
    sound.m_bitDepth = wavData.formatData.bitsPerSample;
    sound.m_soundData = std::move(wavData.soundData);
    
    return sound;
}

Sound::Sound()
    : m_soundReproductionType(SoundReproductionType::Mono)
    , m_sampleFreq(0)
    , m_bitDepth(0)
{
}

ARX_NAMESPACE_END
