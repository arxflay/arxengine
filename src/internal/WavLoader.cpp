#include "internal/WavLoader.h"
#include <fstream>
#include <sstream>
#include "internal/DataExtractor.h"
#include <filesystem>
ARX_NAMESPACE_BEGIN

constexpr std::string_view FMT_HEADER("fmt ");
constexpr size_t UNCOMPRESSED_WAV_FORMAT = 0x0001;
constexpr size_t MONO = 0x0001;
constexpr size_t STEREO = 0x0002;
constexpr size_t MIN_FMT_LIN = 16;
WavLoader::WavLoadCode GetFormatData(DataExtractor &extractor, WavFormatData &formatData)
{
    if (extractor.Extract<std::string_view>(FMT_HEADER.size()).compare(FMT_HEADER) != 0)
        return WavLoader::WavLoadCode::MissingHeader;
    
    uint32_t formatLen = extractor.Extract<uint32_t>();
    if (formatLen < MIN_FMT_LIN)
        return WavLoader::WavLoadCode::InvalidData;
    
    uint16_t wavType = extractor.Extract<uint16_t>();
    if (wavType != UNCOMPRESSED_WAV_FORMAT) //Accept only MICROSOFT UNCOMPRESSED PCM
        return WavLoader::WavLoadCode::UnsupportedFeature;     
    
    formatData.monoStereoFlag = extractor.Extract<uint16_t>();
    if (formatData.monoStereoFlag != MONO && formatData.monoStereoFlag != STEREO)
        return WavLoader::WavLoadCode::InvalidData;
    
    formatData.sampleFreq = extractor.Extract<uint32_t>();
    if (formatData.sampleFreq == 0)
        return WavLoader::WavLoadCode::InvalidData;
    
    formatData.bytePerSec = extractor.Extract<uint32_t>();
    formatData.blockAlignment = extractor.Extract<uint16_t>();
    formatData.bitsPerSample = extractor.Extract<uint16_t>();
    if (formatData.bytePerSec == 0 || formatData.bytePerSec != (formatData.bitsPerSample >> 3) * formatData.sampleFreq)
        return WavLoader::WavLoadCode::InvalidData; 
    if (formatLen > MIN_FMT_LIN) 
    {
        uint16_t extraFormatLen = extractor.Extract<uint16_t>();
        if (extraFormatLen != 0)
            formatData.extendedData = extractor.Extract<uint8_t>(extraFormatLen);
    }

    return WavLoader::WavLoadCode::NoError;
}

constexpr std::string_view FACT_HEADER("fact");
WavLoader::WavLoadCode GetFact(DataExtractor &extractor, std::optional<std::vector<uint8_t>> &factData) 
{
    if (extractor.Peek(FACT_HEADER.size()).compare(FACT_HEADER) != 0)
        return WavLoader::WavLoadCode::MissingHeader;
    
    uint32_t factSize = extractor.Extract<uint32_t>();
    if (factSize == 0)
        return WavLoader::WavLoadCode::InvalidData;
    
    factData = extractor.Extract<uint8_t>(factSize);
    return WavLoader::WavLoadCode::NoError;
}

constexpr std::string_view CUE_HEADER("cue ");
constexpr std::string_view CUE_DATA_CHUNK_ID_DATA("data");
constexpr std::string_view CUE_DATA_CHUNK_ID_SILENT("sInt");
WavLoader::WavLoadCode GetCues(DataExtractor &extractor, std::optional<std::vector<WavCue>> &cues)
{
    if (extractor.Peek(CUE_HEADER.size()).compare(CUE_HEADER) != 0)
        return WavLoader::WavLoadCode::MissingHeader;

    uint32_t cuesSize = extractor.Extract<uint32_t>();
    if (cuesSize == 0)
        return WavLoader::WavLoadCode::InvalidData;

    uint32_t cuesCount = extractor.Extract<uint32_t>();
    if (cuesCount == 0)
        return (cuesSize == 4) ? WavLoader::WavLoadCode::NoError : WavLoader::WavLoadCode::InvalidData;
    else if (cuesSize - sizeof(cuesCount) != cuesCount * sizeof(WavCue))
        return WavLoader::WavLoadCode::InvalidData;
    
    cues.emplace(cuesCount);
    for (uint32_t i = 0; i < cuesCount; i++)
    {
        WavCue cue;
        cue.id = extractor.Extract<uint32_t>();
        cue.position = extractor.Extract<uint32_t>();
        std::string_view dataChunkIdStr = extractor.Extract<std::string_view>(CUE_DATA_CHUNK_ID_DATA.size());
        if (dataChunkIdStr.compare(CUE_DATA_CHUNK_ID_DATA) == 0)
            cue.dataChunkId = DataChunkId::Data;
        else if (dataChunkIdStr.compare(CUE_DATA_CHUNK_ID_SILENT) == 0)
            cue.dataChunkId = DataChunkId::Silent;
        else
            return WavLoader::WavLoadCode::InvalidData;
       
        cue.chunkStart = extractor.Extract<uint32_t>();
        cue.blockStart = extractor.Extract<uint32_t>();
        cue.sampleStart = extractor.Extract<uint32_t>();
        cues.value()[i] = std::move(cue);
    }

    return WavLoader::WavLoadCode::NoError;
}

constexpr std::string_view PLAYLIST_HEADER("plst");
WavLoader::WavLoadCode GetPlaylistData(DataExtractor &extractor, std::optional<std::vector<PlaylistSegment>> &segments)
{
    if (extractor.Peek(PLAYLIST_HEADER.size()).compare(PLAYLIST_HEADER) != 0)
        return WavLoader::WavLoadCode::MissingHeader;
    
    uint32_t playlistSize = extractor.Extract<uint32_t>();
    if (playlistSize == 0)
        return WavLoader::WavLoadCode::InvalidData;

    uint32_t segmentsCount = extractor.Extract<uint32_t>();
    if (segmentsCount == 0)
        return (playlistSize == 4) ? WavLoader::WavLoadCode::NoError : WavLoader::WavLoadCode::InvalidData;
    else if (playlistSize - sizeof(segmentsCount) != segmentsCount * sizeof(PlaylistSegment))
        return WavLoader::WavLoadCode::InvalidData;

    segments.emplace(segmentsCount);
    for (uint32_t i = 0; i < segmentsCount; i++)
    {
        PlaylistSegment segment;
        segment.cueId = extractor.Extract<uint32_t>();
        segment.length = extractor.Extract<uint32_t>();
        segment.repeats = extractor.Extract<uint32_t>();
        segments.value()[i] = std::move(segment);    
    }
    return WavLoader::WavLoadCode::NoError;
}

constexpr std::string_view DATA_HEADER("data");
WavLoader::WavLoadCode GetData(DataExtractor &extractor, std::vector<uint8_t> &out)
{
    if (extractor.Extract<std::string_view>(DATA_HEADER.size()).compare(DATA_HEADER) != 0)
        return WavLoader::WavLoadCode::MissingHeader;

    uint32_t dataSize = extractor.Extract<uint32_t>();
    out = extractor.Extract<uint8_t>(dataSize);
    return WavLoader::WavLoadCode::NoError;
}

/*static*/ WavLoader::WavLoadCode WavLoader::LoadWAVFile(std::string_view filename, WavData &wavData)
{
    if (!std::filesystem::is_regular_file(filename))
        return WavLoadCode::FailedToOpenFile;
    
    std::ifstream file(filename.data(), std::ios::binary | std::ios::in);
    if (file.fail() || file.bad())
        return WavLoadCode::FailedToOpenFile;

    //could use stringstream, but somehow this is faster
    file.seekg(0, std::ios::end);
    std::vector<uint8_t> binaryData(file.tellg() < 0 ? 0 : static_cast<size_t>(file.tellg()));
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(binaryData.data()), static_cast<std::streamsize>(binaryData.size()));
    file.close();

    if (binaryData.empty())
        return WavLoadCode::EmptyFile;

    return WavLoader::LoadWAVBinary(binaryData, wavData);
}

constexpr std::string_view RIFF_HEADER("RIFF");
constexpr std::string_view WAVE_HEADER("WAVE");
/*static*/ WavLoader::WavLoadCode WavLoader::LoadWAVBinary(const std::vector<uint8_t> &data, WavData &wavData)
{
    DataExtractor extractor(data.data(), data.size());
    wavData = {};
    try
    {
        if (extractor.Extract<std::string_view>(RIFF_HEADER.size()).compare(RIFF_HEADER) != 0)
            return WavLoadCode::MissingHeader;

        uint32_t fileSize = extractor.Extract<uint32_t>();
        if (fileSize + sizeof(fileSize) + RIFF_HEADER.size() != data.size())
            return WavLoadCode::InvalidData;
        
        if (extractor.Extract<std::string_view>(WAVE_HEADER.size()).compare(WAVE_HEADER) != 0)
            return WavLoadCode::MissingHeader;

        WavLoadCode st = GetFormatData(extractor, wavData.formatData);
        if (st != WavLoadCode::NoError)
            return st;

        st = GetFact(extractor, wavData.factData);
        if (st != WavLoadCode::NoError && st != WavLoadCode::MissingHeader)
            return st;

        st = GetCues(extractor, wavData.cues);
        if (st != WavLoadCode::NoError && st != WavLoadCode::MissingHeader)
            return st;

        st = GetPlaylistData(extractor, wavData.playlistData);
        if (st != WavLoadCode::NoError && st != WavLoadCode::MissingHeader)
            return st;

        st = GetData(extractor, wavData.soundData);
        if (st != WavLoadCode::NoError)
            return st;
    }
    catch(...)
    {
        return WavLoadCode::FailedToExtractData;
    }

    return WavLoadCode::NoError;
}

ARX_NAMESPACE_END
