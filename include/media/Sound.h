#ifndef ARX_SOUND_H
#define ARX_SOUND_H
#include "ArxDefines.h"
#include <vector>
#include <string_view>

ARX_NAMESPACE_BEGIN

class Sound
{
public:
    enum class Format
    {
        UncompressedWAV
    };

    enum class SoundReproductionType : uint16_t
    {
        Mono = 0x0001,
        Stereo = 0x0002
    };

    SoundReproductionType GetSoundReproductionType() const;
    uint32_t GetSampleFrequency() const;
    uint16_t GetBitDepth() const;
    const std::vector<uint8_t> &GetData() const; 
    
    bool IsInvalid() const;
    
    //only uncompressed wav
    static Sound LoadFromFile(std::string_view filename, Format format);
    static Sound LoadFromBinary(const std::vector<uint8_t> &data, Format format);
private:
    Sound();
    SoundReproductionType m_soundReproductionType;
    uint32_t m_sampleFreq;
    uint16_t m_bitDepth; 
    std::vector<uint8_t> m_soundData;
};

ARX_NAMESPACE_END
#endif
