#include "Utils.h"
#include <vector>
#include <string_view>
#include <string>
#include <iostream>
#include <algorithm>
#include <array>
#include <numeric>
#include <cstring>
#include <charconv>
ARX_NAMESPACE_BEGIN
constexpr std::string_view BASE64_CHARSET = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
constexpr char BASE64_PADDING_CHAR = '=';
constexpr size_t DECODED_SEQUENCE_LEN = 3;
constexpr size_t ENCODED_SEQUENCE_LEN = 4;

constexpr uint8_t INVALID_BASE64_SYMBOL = 255;
std::array<uint8_t, 256> CreateBase64DecodingTable()
{
    std::array<uint8_t, 256> out{};
    std::fill(out.begin(), out.end(), INVALID_BASE64_SYMBOL);
    std::iota(out.begin() + static_cast<size_t>('A'), out.begin() + static_cast<size_t>('Z') + 1, 0);
    std::iota(out.begin() + static_cast<size_t>('a'), out.begin() + static_cast<size_t>('z') + 1, sizeof("ABCDEFGHIJKLMNOPQRSTUVWXYZ") - 1);
    std::iota(out.begin() + static_cast<size_t>('0'), out.begin() + static_cast<size_t>('9') + 1, sizeof("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz") - 1);
    out['+'] = 62;
    out['/'] = 63;

    return out;
}

uint8_t Base64CharToByte(char base64Char)
{
    static auto decodingTable = CreateBase64DecodingTable();
    uint8_t decoded = decodingTable[static_cast<uint8_t>(base64Char)];

    return decoded;
}

/*unsafe*/
static inline size_t Base64ApproximateLenWithoutPadding(std::string_view encData)
{
    if(encData[encData.size() - 2] == BASE64_PADDING_CHAR)
        return encData.size() - 2;
    else if (encData[encData.size() - 1] == BASE64_PADDING_CHAR)
        return encData.size() - 1;

    return encData.size();
}

/*unsafe*/
static inline size_t Base64ApproximateDecodedSize(std::string_view encData)
{
    return (encData.size() >> 2) * DECODED_SEQUENCE_LEN + encData.size() - Base64ApproximateLenWithoutPadding(encData);
}

static inline Utils::Base64DecodeErrCode Base64DecodeInternal(std::string_view encData, uint8_t *out)
{
    size_t len = Base64ApproximateLenWithoutPadding(encData);
    Utils::Base64DecodeErrCode errCode = Utils::Base64DecodeErrCode::NoError;
    for (size_t i = 0; i < len; i++)
    {
        size_t bytePos = (i + 1) % ENCODED_SEQUENCE_LEN;
        size_t sequencePos = (i / ENCODED_SEQUENCE_LEN) * DECODED_SEQUENCE_LEN;
        uint8_t base64Byte = Base64CharToByte(encData[i]);
        if (base64Byte == INVALID_BASE64_SYMBOL)
        {
            errCode = Utils::Base64DecodeErrCode::InvalidBase64String;
            break;
        }

        if (bytePos == 1)
           out[sequencePos] = static_cast<uint8_t>(base64Byte << 2);
        else if (bytePos == 2)
        {
            out[sequencePos] |= (base64Byte & 0x30) >> 4;
            out[sequencePos + 1] = static_cast<uint8_t>((base64Byte & 0x0F) << 4);
        }
        else if (bytePos == 3)
        {
            out[sequencePos + 1] |= (base64Byte & 0x3C) >> 2;
            out[sequencePos + 2] = static_cast<uint8_t>((base64Byte & 0x03) << 6);
        }
        else
            out[sequencePos + 2] |= base64Byte;
    }

    return errCode;
}

/*static*/ Utils::Base64DecodeErrCode Utils::Base64Decode(std::string_view encData, std::string &out)
{
    if (encData.size() == 0)
        return Base64DecodeErrCode::EmptyString; //empty
    else if (encData.size() % ENCODED_SEQUENCE_LEN)
        return Base64DecodeErrCode::InvalidSize; //invalid size
    
    std::string temp;
    temp.resize(Base64ApproximateDecodedSize(encData));
    Base64DecodeErrCode res = Base64DecodeInternal(encData, reinterpret_cast<uint8_t*>(temp.data()));
    if (res == Base64DecodeErrCode::NoError)
        out = std::move(temp);

    return res;
}

/*static*/ Utils::Base64DecodeErrCode Utils::Base64Decode(std::string_view encData, std::vector<uint8_t> &out)
{
    if (encData.size() == 0)
        return Base64DecodeErrCode::EmptyString; //empty
    else if (encData.size() % ENCODED_SEQUENCE_LEN)
        return Base64DecodeErrCode::InvalidSize; //invalid sizee
    
    std::vector<uint8_t> temp(Base64ApproximateDecodedSize(encData));
    Base64DecodeErrCode res = Base64DecodeInternal(encData, temp.data());
    if (res == Base64DecodeErrCode::NoError)
        out = std::move(temp);

    return res;
}

/*static*/ std::string Utils::Base64Encode(std::string_view data)
{
    return Base64Encode(reinterpret_cast<const uint8_t*>(data.data()), data.size());
}

/*static*/ std::string Utils::Base64Encode(const std::vector<uint8_t> &data)
{
    return Base64Encode(data.data(), data.size());
}

/*static*/ std::string Utils::Base64Encode(const uint8_t *data, size_t size)
{
    if (size == 0 || !data)
        return {};

    std::string encryptedData;
    encryptedData.resize(ENCODED_SEQUENCE_LEN * !!(size % DECODED_SEQUENCE_LEN) + ((size / DECODED_SEQUENCE_LEN) << 2));
    for (size_t i = 0; i < size; i++)
    {
        size_t bytePos = (i + 1) % DECODED_SEQUENCE_LEN;
        size_t base64SequencePos = (i / DECODED_SEQUENCE_LEN) * ENCODED_SEQUENCE_LEN;
        if (bytePos == 1)
        {
            encryptedData[base64SequencePos] = BASE64_CHARSET[(data[i] & 0xFD) >> 2];
            encryptedData[base64SequencePos + 1] = static_cast<char>((data[i] & 0x03) << 4);
        }
        else if (bytePos == 2)
        {
            encryptedData[base64SequencePos + 1] = BASE64_CHARSET[static_cast<uint8_t>(encryptedData[base64SequencePos + 1]) + ((data[i] & 0xF0) >> 4)];
            encryptedData[base64SequencePos + 2] = static_cast<char>((data[i] & 0x0F) << 2);
        }
        else
        {
            encryptedData[base64SequencePos + 2] = BASE64_CHARSET[static_cast<uint8_t>(encryptedData[base64SequencePos + 2]) + ((data[i] & 0xC0) >> 6)];
            encryptedData[base64SequencePos + 3] = BASE64_CHARSET[data[i] & 0x3F];
        }
    }

    size_t paddingLen = (!!(size % DECODED_SEQUENCE_LEN)) * DECODED_SEQUENCE_LEN - size % DECODED_SEQUENCE_LEN;
    if (paddingLen != 0)
        encryptedData[encryptedData.size() - paddingLen - 1] = BASE64_CHARSET[static_cast<uint8_t>(encryptedData[encryptedData.size() - paddingLen - 1])];
    std::fill(encryptedData.rbegin(), encryptedData.rbegin() + static_cast<long>(paddingLen), BASE64_PADDING_CHAR);

    return encryptedData;
}

/*static*/ Utils::HexConversionErrorCode Utils::GetByteFromHexStr(std::string_view hexStr, uint8_t &byte)
{
    if (std::from_chars(hexStr.begin(), hexStr.begin() + 2, byte, 16).ec != std::errc{})
        return Utils::HexConversionErrorCode::InvalidHexString;
    
    return Utils::HexConversionErrorCode::NoError;
}

/*static*/ Utils::HexConversionErrorCode Utils::HexStrToBin(std::string_view hexStr, uint8_t *data, size_t len)
{
    if (hexStr.empty())
        return {};
    
    size_t i = 0;
    if (hexStr[0] == '#')
        i = 1;
    
    size_t minLen = std::min(((hexStr.size() - i) >> 1), len) << 1;

    Utils::HexConversionErrorCode err = Utils::HexConversionErrorCode::NoError;
    for (; i < minLen && err == HexConversionErrorCode::NoError; i+= 2)
        err = GetByteFromHexStr(hexStr.substr(i), data[i >> 1]);

    return err;
}

ARX_NAMESPACE_END
