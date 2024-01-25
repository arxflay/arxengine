#pragma once
#include <cstdint>
#include <optional>
#include <vector>
#include <string_view>
#include "ArxDefines.h"
#include "EnumHelpers.h"

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

enum class DataChunkId : uint32_t
{
    Data,
    Silent
};

//https://www.recordingblogs.com/wiki/cue-chunk-of-a-wave-file
struct WavCue //The cue chunk specifies cues / markers in the wave file, such as the beginning of a verse or the end of a solo.
{
    uint32_t id;
    uint32_t position;
    DataChunkId dataChunkId;
    uint32_t chunkStart;
    uint32_t blockStart;
    uint32_t sampleStart;
};

//https://www.recordingblogs.com/wiki/playlist-chunk-of-a-wave-file
struct PlaylistSegment //defines in what order the segments should be played 
{
    uint32_t cueId;
    uint32_t length;
    uint32_t repeats;
};

struct WavData
{
    WavFormatData formatData;
    std::optional<std::vector<uint8_t>> factData; //Fact chunks exist in all wave files that are compressed or that have a wave list chunk
    std::optional<std::vector<WavCue>> cues;
    std::optional<std::vector<PlaylistSegment>> playlistData;
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
        InvalidFormatData = 7,
        InvalidCueData = 8,
        InvalidPlaylistData = 9,
        InvalidFactData = 10
    };
    
    static ENUM_FROM_STRING_DECLARE(WavLoadCode);
    static ENUM_TO_STRING_DECLARE(WavLoadCode);
    

    static WavLoadCode LoadWAVFile(std::string_view filename, WavData &wavData);
    static WavLoadCode LoadWAVBinary(const std::vector<uint8_t> &data, WavData &wavData);
    WavLoader() = delete;
};


ARX_NAMESPACE_END
