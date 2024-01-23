#include <gtest/gtest.h>
#include <Image.h>
#include <Utils.h>
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include "testdefs.h"
#include <Sound.h>

ARX_NAMESPACE_USE;

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}

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
    Image img2 = Image::LoadFromBinary(vec);
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

TEST(Image, PositiveLoadImageFilePNG)
{
    std::filesystem::path testPngPath(TEST_DATA_PATH / std::filesystem::path("test_png.png"));
    Image img(Image::LoadFromFile(testPngPath.native()));
    ASSERT_GT(img.GetData().size(), static_cast<size_t>(0));
    ASSERT_EQ(img.GetSize().x, 32);
    ASSERT_EQ(img.GetSize().y, 33);
    ASSERT_EQ(img.GetColorChannels(), 4);
    ASSERT_EQ(img.GetGLColorChannels(), GL_RGBA);
}

TEST(Image, PositiveLoadImageFileJPEG)
{
    std::filesystem::path testJpgPath(TEST_DATA_PATH / std::filesystem::path("test_jpg.jpg"));
    Image img(Image::LoadFromFile(testJpgPath.native()));
    ASSERT_GT(img.GetData().size(), static_cast<size_t>(0));
    ASSERT_EQ(img.GetSize().x, 32);
    ASSERT_EQ(img.GetSize().y, 33);
    ASSERT_EQ(img.GetColorChannels(), 3);
    ASSERT_EQ(img.GetGLColorChannels(), GL_RGB);
}

TEST(Image, PositiveLoadImageBinary)
{
    std::filesystem::path testJpgPath(TEST_DATA_PATH / std::filesystem::path("test_jpg.jpg"));
    Image img(Image::LoadFromFile(testJpgPath.native()));
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
    const uint8_t *jpegFileContentBytePtr = reinterpret_cast<const uint8_t*>(jpegFileContent.c_str());
    std::vector<uint8_t> jpegFileContentVec(jpegFileContentBytePtr, jpegFileContentBytePtr + jpegFileContent.size());
    Image img2 = Image::LoadFromBinary(jpegFileContentVec);
    ASSERT_EQ(img, img2);
}

TEST(Image, NegativeLoadImageFile)
{
    std::filesystem::path testJpgPath(TEST_DATA_PATH);
    Image img(Image::LoadFromFile(testJpgPath.native()));
    ASSERT_EQ(img.GetData().size(), static_cast<size_t>(0));
    ASSERT_EQ(img.GetSize().x, 0);
    ASSERT_EQ(img.GetSize().y, 0);
    ASSERT_EQ(img.GetColorChannels(), 0);
    ASSERT_EQ(img.GetGLColorChannels(), GL_INVALID_VALUE);
}

TEST(Sound, PositiveLoadFromFile1)
{
    std::filesystem::path testWavPath(TEST_DATA_PATH / std::filesystem::path("test_wav_8bit_44khz.wav"));
    Sound snd(Sound::LoadFromFile(testWavPath.native(), Sound::Format::UncompressedWAV));
    ASSERT_FALSE(snd.IsInvalid());
    ASSERT_EQ(snd.GetBitDepth(), 8);
    ASSERT_EQ(snd.GetSampleFrequency(), 44100);
    ASSERT_EQ(snd.GetSoundReproductionType(), Sound::SoundReproductionType::Mono);
}

TEST(Sound, PositiveLoadFromFile2)
{
    std::filesystem::path testWavPath(TEST_DATA_PATH / std::filesystem::path("test_wav_16bit_11khz.wav"));
    Sound snd(Sound::LoadFromFile(testWavPath.native(), Sound::Format::UncompressedWAV));
    ASSERT_FALSE(snd.IsInvalid());
    ASSERT_EQ(snd.GetBitDepth(), 16);
    ASSERT_EQ(snd.GetSampleFrequency(), 11025);
    ASSERT_EQ(snd.GetSoundReproductionType(), Sound::SoundReproductionType::Mono);
}

TEST(Sound, NegativeLoadFromFile)
{
    std::filesystem::path testWavPath(TEST_DATA_PATH);
    Sound snd(Sound::LoadFromFile(testWavPath.native(), Sound::Format::UncompressedWAV));
    ASSERT_TRUE(snd.IsInvalid());
    ASSERT_EQ(snd.GetBitDepth(), 0);
    ASSERT_EQ(snd.GetSampleFrequency(), 0);
    ASSERT_EQ(snd.GetSoundReproductionType(), Sound::SoundReproductionType::Mono);
}
