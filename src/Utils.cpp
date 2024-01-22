#include "Utils.h"
#include <vector>
#include <string_view>
#include <string>
#include <iostream>
#include <algorithm>
ARX_NAMESPACE_BEGIN
constexpr std::string_view BASE64_CHARSET = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
constexpr char BASE64_PADDING_CHAR = '=';
constexpr size_t DECODED_SEQUENCE_LEN = 3;
constexpr size_t ENCODED_SEQUENCE_LEN = 4;

constexpr uint8_t Base64CharToByte(char base64Char)
{
    if (base64Char >= 'A' && base64Char <= 'Z')
        return static_cast<uint8_t>(base64Char - 'A');
    else if (base64Char >= 'a' && base64Char <= 'z')
        return static_cast<uint8_t>(static_cast<uint8_t>(base64Char - 'a') + sizeof("ABCDEFGHIJKLMNOPQRSTUVWXYZ") - 1);
    else if (base64Char >= '0' && base64Char <= '9')
        return static_cast<uint8_t>(static_cast<uint8_t>(base64Char - '0') + sizeof("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz") - 1);
    else if (base64Char == '+')
        return 62;
    else if (base64Char == '/')
        return 63;

    throw std::runtime_error("Invalid base64 string");
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
    try
    {
        size_t len = Base64ApproximateLenWithoutPadding(encData);

        for (size_t i = 0; i < len; i++)
        {
            size_t bytePos = (i + 1) % ENCODED_SEQUENCE_LEN;
            size_t sequencePos = (i / ENCODED_SEQUENCE_LEN) * DECODED_SEQUENCE_LEN;
            uint8_t base64Byte = Base64CharToByte(encData[i]);
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
    }
    catch(...)
    {
        return Utils::Base64DecodeErrCode::InvalidBase64String; //error during conversion
    }

    return Utils::Base64DecodeErrCode::NoError; 
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

ARX_NAMESPACE_END
