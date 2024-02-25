#include <gtest/gtest.h>
#include <misc/Utils.h>

#include <media/Image.h>
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include "testdefs.h"
#include <media/Sound.h>
#include <logging/FileLogger.h>
#include <GameApp.h>
#include <media/SoundPlayer.h>
#include <ui/Color.h>
#include <ui/ArxWindow.h>
#include <Timer.h>

ARX_NAMESPACE_USE;

int main(int argc, char **argv)
{
    IMPLEMENT_GAMEAPP_NO_MAIN_WITH_LOGGER_INSTANCE(GameApp, ret, std::make_unique<FileLogger>(Logger::LoggingLevel::Debug, "/tmp/log.txt")); 
    if (ret != static_cast<int>(ArxException::ErrorCode::NoError))
        return ret;
    
    ::testing::InitGoogleTest(&argc, argv);
    ret = RUN_ALL_TESTS();
    return ret;
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

TEST(SoundPlayer, DISABLED_PositivePlay)
{
    std::filesystem::path testWavPath(TEST_DATA_PATH / std::filesystem::path("test_wav_8bit_44khz.wav"));
    Sound snd(Sound::LoadFromFile(testWavPath.native(), Sound::Format::UncompressedWAV));

    SoundPlayer player;
    player.LoadSound(snd);
    player.Play(SoundPlayer::PlayMode::Sync, false);
}

TEST(SoundPlayer, DISABLED_PositivePlayMultiple)
{
    std::filesystem::path testWavPath(TEST_DATA_PATH / std::filesystem::path("test_wav_8bit_44khz.wav"));
    Sound snd(Sound::LoadFromFile(testWavPath.native(), Sound::Format::UncompressedWAV));

    SoundPlayer player;
    player.LoadSound(snd);
    player.Play(SoundPlayer::PlayMode::Sync, false);
    
    SoundDeviceContextPair *pair = nullptr;
    EXPECT_NO_THROW(pair = CreateSoundDeviceContextPair("HyperX 7.1 Audio Analog Stereo"));
    SoundPlayer player2(pair);
    player2.LoadSound(snd);
    player2.Play(SoundPlayer::PlayMode::Sync, false);
}

TEST(Hex, PositiveHexToByte)
{
    uint8_t byte = 0;
    auto errCode = Utils::GetByteFromHexStr("30", byte);
    ASSERT_EQ(errCode, Utils::HexConversionErrorCode::NoError);
    ASSERT_EQ(byte, 0x30);
}

TEST(Hex, PositiveHexToString)
{
    std::string helloString;
    helloString.resize(5);
    auto errCode = Utils::HexStrToBin("68656c6c6f", reinterpret_cast<uint8_t*>(helloString.data()), helloString.size());
    ASSERT_EQ(errCode, Utils::HexConversionErrorCode::NoError);
    ASSERT_STREQ(helloString.data(), "hello");
}

TEST(Hex, NegativeHexToString)
{
    std::string helloString;
    helloString.resize(5);
    auto errCode = Utils::HexStrToBin("#68656c6cHJ", reinterpret_cast<uint8_t*>(helloString.data()), helloString.size());
    ASSERT_EQ(errCode, Utils::HexConversionErrorCode::InvalidHexString);
    ASSERT_STRNE(helloString.data(), "hello");
}

TEST(Color, PositiveHexToColor1)
{
    Color c("#FFC310");
    ASSERT_EQ(c.r, 0xFF);
    ASSERT_EQ(c.g, 0xC3);
    ASSERT_EQ(c.b, 0x10);
    ASSERT_EQ(c.a, 0xFF);
}

TEST(Color, PositiveHexToColor2)
{
    Color c("#FFC310C0");
    ASSERT_EQ(c.r, 0xFF);
    ASSERT_EQ(c.g, 0xC3);
    ASSERT_EQ(c.b, 0x10);
    ASSERT_EQ(c.a, 0xC0);
}

TEST(Color, PositiveColorToNormalizedColor)
{
    Color c(128, 255, 30, 11);
    ASSERT_EQ(c.GetNormalizedColorRGBA(), glm::vec4(c.r / 255.0, c.g / 255.0, c.b / 255.0, c.a / 255.0));
}

TEST(ArxWindow, DISABLED_PositiveShowWin)
{
    ArxWindow *win = new ArxWindow("test", Size(300, 300), Position(8, 6));
    win->Show();
    GameApp::GetGlobalApp()->Run();
}

TEST(ArxWindow, DISABLED_PositiveRainbowWin)
{
    ArxWindow *win = new ArxWindow("test", Size(300, 300));
    win->Show();
    Timer *t = new Timer(win);
    t->GetEventManager().Bind<TimerEvent>([win](TimerEvent &){win->SetBackgroundColor(arx::Color((uint)rand() % 255, (uint)rand() % 255, (uint)rand() % 255)); win->Draw(); });
    t->SetInterval(std::chrono::seconds(1));
    t->Start(Timer::TimerType::CONTINUOUS);

    GameApp::GetGlobalApp()->Run();
}

TEST(ArxWindow, DISABLED_PositiveEndAfter2Seconds)
{
    ArxWindow *win = new ArxWindow("test", Size(300, 300));
    win->Show();
    Timer *t = new Timer(win);
    t->GetEventManager().Bind<TimerEvent>([errCode=0](TimerEvent &){ GameApp::GetGlobalApp()->Exit(errCode); });
    t->SetInterval(std::chrono::seconds(1));
    t->Start(Timer::TimerType::SINGLE_FIRE);
    GameApp::GetGlobalApp()->Run();
}

TEST(ArxWindow, PositiveMultiWindow)
{
    ArxWindow *win = new ArxWindow("test", Size(300, 300));
    win->Show();
    Timer *t = new Timer(win);
    t->GetEventManager().Bind<TimerEvent>([win](TimerEvent &){win->SetBackgroundColor(arx::Color((uint)rand() % 255, (uint)rand() % 255, (uint)rand() % 255)); win->Draw(); });
    t->SetInterval(std::chrono::seconds(1));
    t->Start(Timer::TimerType::CONTINUOUS);
    /*
    ArxWindow *win2 = new ArxWindow("test", Size(300, 300), Position(300, 1));
    win2->Show();
    Timer *t2 = new Timer(win2);
    t2->GetEventManager().Bind<TimerEvent>([win2](TimerEvent &){
        win2->SetBackgroundColor(arx::Color((uint)rand() % 255, (uint)rand() % 255, (uint)rand() % 255)); 
        win2->SetPosition(Position(win2->GetPosition().x + 10, win2->GetPosition().y)); 
        win2->Draw(); 
    });
    t2->SetInterval(std::chrono::seconds(1));
    t2->Start(Timer::TimerType::CONTINUOUS);*/

    GameApp::GetGlobalApp()->Run();
}

