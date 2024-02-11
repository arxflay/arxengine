#include "internal/WavLoader.h"
#include <fstream>
#include <sstream>
#include "internal/DataExtractor.h"
#include <filesystem>
#include <array>
#include <logging/Logger.h>
ARX_NAMESPACE_BEGIN

ENUM_FROM_TO_STRING_DEFINE_NESTED(WavLoader, WavLoadCode, 
        "NoError", "FailedToOpenFile", "EmptyFile", "MissingHeader", "UnsupportedFeature", 
        "FailedToExtractData", "InvalidData", "InvalidFormatData");

uint32_t CalculateBytePerSec(uint16_t bitsPerSample, uint16_t monoStereoFlag, uint32_t sampleFreq)
{
    return (bitsPerSample >> 3) * sampleFreq * monoStereoFlag;
}

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
        return WavLoader::WavLoadCode::InvalidFormatData;
    
    uint16_t wavType = extractor.Extract<uint16_t>();
    if (wavType != UNCOMPRESSED_WAV_FORMAT) //Accept only MICROSOFT UNCOMPRESSED PCM
        return WavLoader::WavLoadCode::UnsupportedFeature;     
    
    formatData.monoStereoFlag = extractor.Extract<uint16_t>();
    if (formatData.monoStereoFlag != MONO && formatData.monoStereoFlag != STEREO)
        return WavLoader::WavLoadCode::InvalidFormatData;

    formatData.sampleFreq = extractor.Extract<uint32_t>();
    if (formatData.sampleFreq == 0)
        return WavLoader::WavLoadCode::InvalidFormatData;
    
    formatData.bytePerSec = extractor.Extract<uint32_t>();
    formatData.blockAlignment = extractor.Extract<uint16_t>();
    formatData.bitsPerSample = extractor.Extract<uint16_t>();
    uint32_t expectedBytePerSec = CalculateBytePerSec(formatData.bitsPerSample, formatData.monoStereoFlag, formatData.sampleFreq);
    if (formatData.bytePerSec == 0 || formatData.bytePerSec != expectedBytePerSec)
    {
        GLOG->Debug("invalid bytePerSec, expected %d but got %d", formatData.sampleFreq, formatData.bytePerSec);
        return WavLoader::WavLoadCode::InvalidFormatData;
    }
    if (formatLen > MIN_FMT_LIN) 
    {
        uint16_t extraFormatLen = extractor.Extract<uint16_t>();
        if (extraFormatLen != 0)
            formatData.extendedData = extractor.Extract<uint8_t>(extraFormatLen);
    }

    return WavLoader::WavLoadCode::NoError;
}

constexpr std::string_view DATA_HEADER("data");
WavLoader::WavLoadCode SkipNonDataChunks(DataExtractor &extractor)
{
    bool foundNonDataChunk = true;
    while(foundNonDataChunk)
    {
        foundNonDataChunk = false;
        std::string_view chunkName = extractor.Peek(4);
        if (!std::all_of(chunkName.begin(), chunkName.end(), [](char c){ return isalnum(c) || c == ' '; }))
            return WavLoader::WavLoadCode::FailedToExtractData;
        
        if (chunkName.compare(DATA_HEADER) != 0)
        {
            extractor.Skip(4);
            uint32_t chunkSize = extractor.Extract<uint32_t>();
            extractor.Skip(chunkSize);
            foundNonDataChunk = true;
        }
    }

    return WavLoader::WavLoadCode::NoError;
}

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

        st = SkipNonDataChunks(extractor);
        if (st != WavLoadCode::NoError)
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
