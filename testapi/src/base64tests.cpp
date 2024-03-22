#include <gtest/gtest.h>
#include <arxengine/misc/Utils.h>
#include <arxengine/media/Image.h>
#include <filesystem>
#include <fstream>
//#include "testdefs.h"

ARX_NAMESPACE_USE;
#ifdef TEST_BASE64

TEST(Base64, PositiveBase64Decode1)
{
    std::string str;
    ASSERT_EQ(Utils::Base64Decode("TWFu", str), Utils::Base64DecodeErrCode::NoError);
    ASSERT_STREQ(str.c_str(), "Man");
}

TEST(Base64, PositiveBase64Decode2)
{
    std::string str;
    ASSERT_EQ(Utils::Base64Decode("TWE=", str), Utils::Base64DecodeErrCode::NoError);
    ASSERT_STREQ(str.c_str(), "Ma");
}

TEST(Base64, PositiveBase64Decode3)
{
    std::string str;
    ASSERT_EQ(Utils::Base64Decode("TQ==", str), Utils::Base64DecodeErrCode::NoError);
    ASSERT_STREQ(str.c_str(), "M");
}

TEST(Base64, PositiveBase64Decode4)
{
    std::string str;
    ASSERT_EQ(Utils::Base64Decode("TWFuTWFu", str), Utils::Base64DecodeErrCode::NoError);
    ASSERT_STREQ(str.c_str(), "ManMan");
}

TEST(Base64, PositiveBase64DecEncDecode)
{
    std::filesystem::path testJpgPath(TEST_DATA_PATH / std::filesystem::path("test_jpg.jpg"));
    std::string jpegFileContent;
    {
        std::ifstream f;
        f.exceptions(std::ifstream::badbit | std::ifstream::failbit);
        f.open(testJpgPath, std::ifstream::binary | std::ifstream::in);
        std::stringstream ss;
        ss << f.rdbuf();
        f.close();
        jpegFileContent = ss.str();
    }
    auto str = Utils::Base64Encode(jpegFileContent);
    std::vector<uint8_t> vec;
    Utils::Base64Decode(str, vec);
    Image img2 = Image::LoadFromBinary(vec.data(), vec.size());
    ASSERT_FALSE(img2.IsInvalid());
}

TEST(Base64, NegativeBase64Decode1)
{
    std::string str;
    ASSERT_EQ(arx::Utils::Base64Decode("", str), Utils::Base64DecodeErrCode::EmptyString);    
}

TEST(Base64, NegativeBase64Decode2)
{
    std::string str;
    ASSERT_EQ(arx::Utils::Base64Decode("xxxxx", str), Utils::Base64DecodeErrCode::InvalidSize);    
}

TEST(Base64, NegativeBase64Decode3)
{
    std::string str;
    ASSERT_EQ(arx::Utils::Base64Decode("x===", str), Utils::Base64DecodeErrCode::InvalidBase64String);    
}

TEST(Base64, PositiveBase64Encode1)
{
    auto enc = Utils::Base64Encode({0x4d, 0x61, 0x6e});
    ASSERT_STREQ(enc.c_str(), "TWFu");
}

TEST(Base64, PositiveBase64Encode2)
{
    auto enc = Utils::Base64Encode("Ma");
    ASSERT_STREQ(enc.c_str(), "TWE=");
}

TEST(Base64, PositiveBase64Encode3)
{
    auto enc = Utils::Base64Encode({0x4d});
    ASSERT_STREQ(enc.c_str(), "TQ==");
}

TEST(Base64, PositiveBase64Encode4)
{
    auto enc = Utils::Base64Encode({0x4d, 0x61, 0x6e, 0x4d, 0x61, 0x6e});
    ASSERT_STREQ(enc.c_str(), "TWFuTWFu");
}

TEST(Base64, NegativeBase64Encode1)
{
    auto enc = Utils::Base64Encode("");
    ASSERT_STREQ(enc.c_str(), "");
}

TEST(Base64, NegativeBase64Encode2)
{
    auto enc = Utils::Base64Encode(nullptr, 1);
    ASSERT_STREQ(enc.c_str(), "");
}

#endif
