#ifndef ARX_UTILS_H
#define ARX_UTILS_H
#include "ArxDefines.h"
#include <vector>
#include <string_view>
#include <string>
#include <cstdio>
ARX_NAMESPACE_BEGIN

class Utils
{
public:
    enum class Base64DecodeErrCode
    {
        NoError,
        EmptyString,
        InvalidSize,
        InvalidBase64String
    };
    static Base64DecodeErrCode Base64Decode(std::string_view encData, std::string &out);
    static Base64DecodeErrCode Base64Decode(std::string_view encData, std::vector<uint8_t> &out);
    static std::string Base64Encode(std::string_view data);
    static std::string Base64Encode(const std::vector<uint8_t> &data);
    static std::string Base64Encode(const uint8_t *data, size_t size);

    //return empty string on error
    template<typename ...Args>
    static std::string Format(const char *format, Args&& ...args)
    {
        std::string out;
        //https://en.cppreference.com/w/cpp/io/c/fprintf
        //Calling std::snprintf with zero buf_size and null pointer for buffer is useful 
        //(when the overhead of double-call is acceptable) to determine the necessary buffer size to contain the output
        int len = snprintf(nullptr, 0, format, std::forward<Args&&>(args)...);
        if (len != -1)
        {
            out.resize(static_cast<size_t>(len));
            sprintf(out.data(), format, std::forward<Args&&>(args)...);
        }
        return out;
    }
   
    enum class HexConversionErrorCode
    {
        NoError,
        InvalidHexString
    };

    static HexConversionErrorCode GetByteFromHexStr(std::string_view hexStr, uint8_t &byte);
    static HexConversionErrorCode HexStrToBin(std::string_view hexStr, uint8_t *data, size_t len);
    
    enum class LoadFileErrorCode
    {
        NoError,
        FailedToOpenFile
    };

    static LoadFileErrorCode LoadBinaryFile(std::string_view filename, std::vector<uint8_t> &data);
    static LoadFileErrorCode LoadFile(std::string_view filename, std::string &data);
};

ARX_NAMESPACE_END

#endif
