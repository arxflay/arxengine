#ifndef ARX_WAVLOADER_H
#define ARX_WAVLOADER_H
#include <cstdint>
#include <optional>
#include <vector>
#include <string_view>
#include "ArxDefines.h"
#include "misc/EnumHelpers.h"

ARX_NAMESPACE_BEGIN
    
//format descibed in https://www.lim.di.unimi.it/IEEE/VROS/RIFF.HTM
struct WavFormatData
{
    uint16_t monoStereoFlag;
    uint32_t sampleFreq; //Per second. Also known as sample rate. Freq that is highier than sampleFreq / 2.5 (Nyquist rate) is cutted
    uint32_t bytePerSec; // (bitsPerSample / 8) * sampleFreq * monoStereoFlag
    uint16_t blockAlignment;
    uint16_t bitsPerSample; 
    std::optional<std::vector<uint8_t>> extendedData;
};

struct WavData
{
    WavFormatData formatData;
    std::vector<uint8_t> soundData;
};

class WavLoader
{
public:
    enum class WavLoadCode
    {
        NoError = 0,
        FailedToOpenFile = 1,
        EmptyFile = 2,
        MissingHeader = 3,
        UnsupportedFeature = 4,
        FailedToExtractData = 5,
        InvalidData = 6,
        InvalidFormatData = 7
    };
    
    static ENUM_FROM_STRING_DECLARE(WavLoadCode);
    static ENUM_TO_STRING_DECLARE(WavLoadCode);
    

    static WavLoadCode LoadWAVFile(std::string_view filename, WavData &wavData);
    static WavLoadCode LoadWAVBinary(const std::vector<uint8_t> &data, WavData &wavData);
    WavLoader() = delete;
};


ARX_NAMESPACE_END

#endif
