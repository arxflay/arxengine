#include <gtest/gtest.h>
#include <arxengine/misc/Utils.h>

#include <arxengine/media/Image.h>
#include <fstream>
#include <sstream>
#include "testdefs.h"
#include <arxengine/media/Sound.h>
#include <arxengine/logging/FileLogger.h>
#include <arxengine/GameApp.h>
#include <arxengine/media/SoundPlayer.h>
#include <cstdlib>
#include <arxengine/ui/Color.h>
#include <arxengine/ui/ArxWindow.h>
#include <arxengine/Timer.h>
#include <arxengine/ui/Painter.h>
#include <arxengine/gl/Texture2D.h>
#include <arxengine/media/Font.h>
#include <arxengine/ui/KeyEvent.h>
#include <arxengine/ArxException.h>
#include <arxengine/ui/ImageControl.h>
#include <arxengine/ui/MouseEvent.h>
#include <arxengine/ui/Label.h>
#include <arxengine/ui/BitmapButton.h>
#include <iostream>
#include <arxengine/Json.h>

ARX_NAMESPACE_USE;

int main(int argc, char **argv)
{
    std::filesystem::path logPath;
    #ifdef WIN32
        logPath = getenv("PROGRAMDATA");
    #else
        logPath = getenv("HOME");
        logPath = logPath / std::filesystem::path(".local/share");
    #endif
    
    logPath = logPath / std::filesystem::path("arxengine");
    std::filesystem::create_directory(logPath);
    logPath = logPath / std::filesystem::path("log.txt");
    IMPLEMENT_GAMEAPP_NO_MAIN_WITH_LOGGER_INSTANCE(GameApp, ret, std::make_unique<FileLogger>(Logger::LoggingLevel::Debug, logPath.u8string()));

    if (ret != static_cast<int>(ArxException::ErrorCode::NoError))
        return ret;
 
    ::testing::InitGoogleTest(&argc, argv);
    ret = RUN_ALL_TESTS();
    return ret;
}

TEST(Image, PositiveLoadImageFilePNG)
{
    std::filesystem::path testPngPath(TEST_DATA_PATH / std::filesystem::path("test_png.png"));
    Image img(Image::LoadFromFile(testPngPath.u8string()));
    ASSERT_GT(img.GetData().size(), static_cast<size_t>(0));
    ASSERT_EQ(img.GetSize(), SizeUL(32, 33));
    ASSERT_EQ(img.GetColorChannels(), 4);
}

TEST(Image, PositiveLoadImageFileJPEG)
{
    std::filesystem::path testJpgPath(TEST_DATA_PATH / std::filesystem::path("test_jpg.jpg"));
    Image img(Image::LoadFromFile(testJpgPath.u8string()));
    ASSERT_GT(img.GetData().size(), static_cast<size_t>(0));
    ASSERT_EQ(img.GetSize(), SizeUL(32, 33));
    ASSERT_EQ(img.GetColorChannels(), 3);
}

TEST(Image, PositiveLoadImageBinary)
{
    std::filesystem::path testJpgPath(TEST_DATA_PATH / std::filesystem::path("test_jpg.jpg"));
    Image img(Image::LoadFromFile(testJpgPath.u8string()));
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
    Image img2 = Image::LoadFromBinary(jpegFileContentVec.data(), jpegFileContentVec.size());
    ASSERT_EQ(img, img2);
}

TEST(Image, NegativeLoadImageFile)
{
    std::filesystem::path testJpgPath(TEST_DATA_PATH);
    Image img(Image::LoadFromFile(testJpgPath.u8string()));
    ASSERT_EQ(img.GetData().size(), static_cast<size_t>(0));
    ASSERT_EQ(img.GetSize(), SizeUL(0, 0));
    ASSERT_EQ(img.GetColorChannels(), 0);
}

TEST(Sound, PositiveLoadFromFile1)
{
    std::filesystem::path testWavPath(TEST_DATA_PATH / std::filesystem::path("test_wav_8bit_44khz.wav"));
    Sound snd(Sound::LoadFromFile(testWavPath.u8string(), Sound::Format::UncompressedWAV));
    ASSERT_FALSE(snd.IsInvalid());
    ASSERT_EQ(snd.GetBitDepth(), 8);
    ASSERT_EQ(snd.GetSampleFrequency(), 44100);
    ASSERT_EQ(snd.GetSoundReproductionType(), Sound::SoundReproductionType::Mono);
}

TEST(Sound, PositiveLoadFromFile2)
{
    std::filesystem::path testWavPath(TEST_DATA_PATH / std::filesystem::path("test_wav_16bit_11khz.wav"));
    Sound snd(Sound::LoadFromFile(testWavPath.u8string(), Sound::Format::UncompressedWAV));
    ASSERT_FALSE(snd.IsInvalid());
    ASSERT_EQ(snd.GetBitDepth(), 16);
    ASSERT_EQ(snd.GetSampleFrequency(), 11025);
    ASSERT_EQ(snd.GetSoundReproductionType(), Sound::SoundReproductionType::Mono);
}

TEST(Sound, NegativeLoadFromFile)
{
    std::filesystem::path testWavPath(TEST_DATA_PATH);
    Sound snd(Sound::LoadFromFile(testWavPath.u8string(), Sound::Format::UncompressedWAV));
    ASSERT_TRUE(snd.IsInvalid());
    ASSERT_EQ(snd.GetBitDepth(), 0);
    ASSERT_EQ(snd.GetSampleFrequency(), 0);
    ASSERT_EQ(snd.GetSoundReproductionType(), Sound::SoundReproductionType::Mono);
}

TEST(SoundPlayer, DISABLED_PositivePlay)
{
    std::filesystem::path testWavPath(TEST_DATA_PATH / std::filesystem::path("test_wav_8bit_44khz.wav"));
    Sound snd(Sound::LoadFromFile(testWavPath.u8string(), Sound::Format::UncompressedWAV));

    SoundPlayer player;
    player.LoadSound(snd);
    player.Play(SoundPlayer::PlayMode::Sync, false);
}

TEST(SoundPlayer, DISABLED_PositivePlayMultiple)
{
    std::filesystem::path testWavPath(TEST_DATA_PATH / std::filesystem::path("test_wav_8bit_44khz.wav"));
    Sound snd(Sound::LoadFromFile(testWavPath.u8string(), Sound::Format::UncompressedWAV));

    SoundPlayer player;
    player.LoadSound(snd);
    player.Play(SoundPlayer::PlayMode::Sync, false);
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

TEST(Json, DISABLED_PositiveJsonInitializerList)
{
    Json j = Json {
        {"name", "test"},
        {"age", 22 },
        {"happy", true },
    };
    
    std::string name;
    int age = 0;
    bool happy = false;
    ASSERT_NO_THROW(name = j["name"].Get<std::string>());
    ASSERT_STREQ(name.data(), "test");
    ASSERT_NO_THROW(age = j["age"].Get<int>());
    ASSERT_EQ(age, 22);
    ASSERT_NO_THROW(happy = j["happy"].Get<bool>());
    ASSERT_TRUE(happy);
}

TEST(Json, PositiveJsonLoadFromFile)
{
    Json j(Json::FromFile((TEST_DATA_PATH / "test.json").u8string())); 
    std::string name;
    int age = 0;
    bool happy = false;
    ASSERT_NO_THROW(name = j["name"].Get<std::string>());
    ASSERT_STREQ(name.data(), "test");
    ASSERT_NO_THROW(age = j["age"].Get<int>());
    ASSERT_EQ(age, 22);
    ASSERT_NO_THROW(happy = j["happy"].Get<bool>());
    ASSERT_TRUE(happy);

    Json &arr = j["testArr"];
    double val0 = arr[0].Get<double>();
    ASSERT_EQ(val0, 10.5);
    int val1 = arr[1]["id"].Get<int>();
    ASSERT_EQ(val1, 0);

    std::cout << Json::ToJsonStr(j) << '\n';
}


TEST(ArxWindow, DISABLED_PositiveShowWin)
{
    ArxWindow *win = new ArxWindow("test", SizeF(300, 300), Position(8, 6));
    win->Show();
    GameApp::GetGlobalApp()->Run();
}

TEST(ArxWindow, DISABLED_PositiveRainbowWin)
{
    ArxWindow *win = new ArxWindow("test", SizeF(300, 300));
    win->Show();
    Timer *t = new Timer(win);
    t->GetEventManager().Bind<TimerEvent>([win](TimerEvent &){
        win->SetBackgroundColor(arx::Color((uint8_t)(rand() % 255), (uint8_t)(rand() % 255), (uint8_t)(rand() % 255)));  
    });
    t->SetInterval(std::chrono::seconds(1));
    t->Start(Timer::TimerType::CONTINUOUS);

    GameApp::GetGlobalApp()->Run();
}

TEST(ArxWindow, DISABLED_PositiveEndAfter2Seconds)
{
    ArxWindow *win = new ArxWindow("test", SizeF(300, 300));
    win->Show();
    Timer *t = new Timer(win);
    t->GetEventManager().Bind<TimerEvent>([errCode=0](TimerEvent &){ GameApp::GetGlobalApp()->Exit(errCode); });
    t->SetInterval(std::chrono::seconds(2));
    t->Start(Timer::TimerType::SINGLE_FIRE);
    GameApp::GetGlobalApp()->Run();
}

TEST(ArxWindow, DISABLED_PositiveMultiWindow)
{
    ArxWindow *win = new ArxWindow("test", SizeF(300, 300));
    win->Show();
    Timer *t = new Timer(win);
    t->GetEventManager().Bind<TimerEvent>([win](TimerEvent &){
        win->SetBackgroundColor(arx::Color((uint8_t)(rand() % 255), (uint8_t)(rand() % 255), (uint8_t)(rand() % 255))); 
    });
    t->SetInterval(std::chrono::seconds(1));
    t->Start(Timer::TimerType::CONTINUOUS);

    ArxWindow *win2 = new ArxWindow("test", SizeF(300, 300), Position(300, 1));
    win2->Show();
    Timer *t2 = new Timer(win2);
    t2->GetEventManager().Bind<TimerEvent>([win2](TimerEvent &){
        win2->SetBackgroundColor(arx::Color((uint8_t)(rand() % 255), (uint8_t)(rand() % 255), (uint8_t)(rand() % 255))); 
        win2->SetPosition(Position(win2->GetPosition().x + 10, win2->GetPosition().y)); 
    });
    t2->SetInterval(std::chrono::seconds(1));
    t2->Start(Timer::TimerType::CONTINUOUS);

    GameApp::GetGlobalApp()->Run();
}

TEST(ArxWindow, DISABLED_PositivePainterTest1)
{
    ArxWindow *win = new ArxWindow("test", SizeF(640, 360));
    win->Show();
    Texture2D *testTexture = new Texture2D(win);
    std::filesystem::path testJpgPath(TEST_DATA_PATH / std::filesystem::path("test_png.png"));
    Image img(Image::LoadFromFile(testJpgPath.u8string()));
    win->SetIcon(img);
    testTexture->SetData(img);
    win->SetFixedViewport(640, 360);
    win->SetWindowAspectRatio(16, 9);
    win->GetEventManager().Bind<DrawEvent>([win, testTexture](DrawEvent &e){
        (void)win;
        Painter painter(e);
        painter.Clear();
        painter.SetBrush(Brush(Color(100, 200, 100)));
        painter.DrawRectangle(Position(60, 60), SizeF(100, 100));
        painter.SetBrush(Brush(Color(10, 50, 100)));
        painter.DrawRectangle(Position(0, 0), SizeF(40, 40));
        painter.SetBrush(Brush(Color(0, 0, 0)));
        painter.DrawRectangle(Position(150, 200), SizeF(60, 60));
        painter.DrawTexture2D(Position(0, 0), SizeF(100, 100), testTexture);
    });

    GameApp::GetGlobalApp()->Run();
}

TEST(ArxWindow, DISABLED_PositivePainterTest2)
{
    ArxWindow *win = new ArxWindow("test", SizeF(300, 300));
    win->Show();
    Texture2D *testTexture = new Texture2D(win);
    std::filesystem::path testJpgPath(TEST_DATA_PATH / std::filesystem::path("test_png.png"));
    Image img(Image::LoadFromFile(testJpgPath.u8string()));
    win->SetIcon(img);
    testTexture->SetData(img);
    win->GetEventManager().Bind<DrawEvent>([win, testTexture](DrawEvent &e){
        (void)win;
        Painter painter(e);
        painter.Clear();
        painter.SetBrush(Brush(Color(100, 200, 100)));
        painter.DrawRectangle(Position(60, 60), SizeF(100, 100));
        painter.SetBrush(Brush(Color(10, 50, 100)));
        painter.DrawRectangle(Position(0, 0), SizeF(40, 40));
        painter.SetBrush(Brush(Color(0, 0, 0)));
        painter.DrawRectangle(Position(150, 200), SizeF(60, 60));
        painter.DrawTexture2D(Position(0, 0), SizeF(100, 100), testTexture);
    });

    GameApp::GetGlobalApp()->Run();
}

TEST(Font, DISABLED_PositiveLoadFont)
{
    std::filesystem::path testFontPath(TEST_DATA_PATH / std::filesystem::path("test-font-roboto.ttf"));
    Font font(Font::LoadFromFile(testFontPath.u8string()));
    font.SetSizeInPt(30);
    ASSERT_FALSE(font.IsInvalid());
    
    ArxWindow *win = new ArxWindow("test", SizeF(640, 360));
    win->Show();
    win->SetFont(std::move(font));
    win->SetFixedViewport(640, 360);
    
    win->GetEventManager().Bind<DrawEvent>([win](DrawEvent &e){
        (void)win;
        Painter painter(e);
        painter.Clear();
        painter.SetPen(Pen(Color(255, 0, 0)));
        win->GetFontCache()->EnableFontSmoothing(false);
        painter.DrawText("Test string", Position(0, 50));
    });

    GameApp::GetGlobalApp()->Run();
}

TEST(ArxWindow, DISABLED_PositiveWindowInput)
{
    ArxWindow *win = new ArxWindow("test", SizeF(640, 360));
    win->Show();
    win->SetFixedViewport(640, 360);
    float position = 20.0f; 

    win->GetEventManager().Bind<KeyDownEvent>([win](KeyDownEvent &e) {  
        if (e.GetKey() == KeyEvent::Key::Esc)
            win->RequestDelete();
    });

    win->GetEventManager().Bind<KeyHoldEvent>([ &position](KeyHoldEvent &e) {
        if (e.GetKey() == KeyEvent::Key::D)
            position += 1000 * static_cast<float>(GameApp::GetGlobalApp()->GetDeltaTime());
        if (e.GetKey() == KeyEvent::Key::A)
            position -= 1000 * static_cast<float>(GameApp::GetGlobalApp()->GetDeltaTime());
    });

    win->GetEventManager().Bind<KeyUpEvent>([win](KeyUpEvent &e) {
        (void)win;
        if (e.GetKey() != KeyEvent::Key::D && e.GetKey() != KeyEvent::Key::A)
            std::cout << e.GetKeyChar() << '\n'; 
    });

    win->GetEventManager().Bind<DrawEvent>([win, &position](DrawEvent &e){
        (void)win;
        Painter painter(e);
        painter.Clear();
        painter.SetBrush(Brush(Color(100, 200, 100)));
        painter.DrawRectangle(Position(position, 60.0f), SizeF(100, 100));

    });

    win->EnableVSync(true);

    GameApp::GetGlobalApp()->Run();
}

TEST(ArxWindow, DISABLED_PositiveImageControl)
{
    ArxWindow *win = new ArxWindow("test", SizeF(640, 360));
    win->Show();
    win->SetFixedViewport(640, 360);
    std::filesystem::path testJpgPath(TEST_DATA_PATH / std::filesystem::path("test_png.png"));
    Image img(Image::LoadFromFile(testJpgPath.u8string()));
    ImageControl *ctrl = new ImageControl(win, img, SizeF(100, 100), Position(100, 100));
    ctrl->SetFilteringMode(Texture::TextureFilteringMode::Linear);
    ctrl->SetBackgroundColor(constants::COLOR_BLACK);

    GameApp::GetGlobalApp()->Run();
}

TEST(ArxWindow, DISABLED_PositiveImageControlHideShow)
{
    ArxWindow *win = new ArxWindow("test", SizeF(640, 360));
    win->Show();
    win->SetFixedViewport(640, 360);
    std::filesystem::path testJpgPath(TEST_DATA_PATH / std::filesystem::path("test_png.png"));
    Image img(Image::LoadFromFile(testJpgPath.u8string()));
    ImageControl *ctrl = new ImageControl(win, img, SizeF(100, 100), Position(100, 100));
    ctrl->SetFilteringMode(Texture::TextureFilteringMode::Linear);
    ctrl->Hide();
    Timer *t = new Timer(win);
    t->GetEventManager().Bind<TimerEvent>([ctrl](TimerEvent &){ ctrl->Show(); });
    t->SetInterval(std::chrono::seconds(1));
    t->Start(Timer::TimerType::SINGLE_FIRE);

    GameApp::GetGlobalApp()->Run();
}

TEST(ArxWindow, DISABLED_PositiveImageControlTileSize)
{
    ArxWindow *win = new ArxWindow("test", SizeF(640, 360));
    win->SetBackgroundColor(Color("#005050"));
    win->Show();
    win->SetFixedViewport(640, 360);
    std::filesystem::path testJpgPath(TEST_DATA_PATH / std::filesystem::path("test_png.png"));
    Image img(Image::LoadFromFile(testJpgPath.u8string()));
    ImageControl *ctrl = new ImageControl(win, img, SizeF(100, 100), Position(100, 100));
    ctrl->SetFilteringMode(Texture::TextureFilteringMode::Linear);
    ctrl->EnableTilingMode(TileData{1, 3});
    ctrl->SetBackgroundColor(constants::COLOR_BLACK);
    win->EnableVSync(true);
    Position pos(100, 100);
    win->GetEventManager().Bind<KeyHoldEvent>([ctrl, &pos](KeyHoldEvent &e) {
        if (e.GetKey() == KeyEvent::Key::W)
            pos.y -= 1000 * static_cast<float>(GameApp::GetGlobalApp()->GetDeltaTime());
        else if (e.GetKey() == KeyEvent::Key::S)
            pos.y += 1000 * static_cast<float>(GameApp::GetGlobalApp()->GetDeltaTime());
        ctrl->SetPosition(pos);
    });
    GameApp::GetGlobalApp()->Run();
}

TEST(ArxWindow, DISABLED_PositiveMouseInput)
{
    ArxWindow *win = new ArxWindow("test", SizeF(640, 360));
    win->Show();
    win->SetFixedViewport(640, 360);
    float position = 20.0f; 

    win->GetEventManager().Bind<MouseDownEvent>([win](MouseDownEvent &e) {  
        if (e.GetMouseButtonType() == MouseButtonEvent::ButtonType::Scroll)
            win->RequestDelete();
    });
    std::filesystem::path testJpgPath(TEST_DATA_PATH / std::filesystem::path("test_png.png"));
    Image img(Image::LoadFromFile(testJpgPath.u8string()));
    ImageControl *ctrl = new ImageControl(win, img, SizeF(100, 100), Position(100, 100));
    ctrl->SetFilteringMode(Texture::TextureFilteringMode::Linear);
    ctrl->SetBackgroundColor(constants::COLOR_BLACK);

    ctrl->GetEventManager().Bind<MouseDownEvent>([win](MouseDownEvent &) {  
        (void)win;
        std::cout << "ImageControlDown" << '\n'; 
    });

    ctrl->GetEventManager().Bind<MouseUpEvent>([win](MouseUpEvent &) {  
        (void)win;
        std::cout << "ImageControlUp" << '\n'; 
    });

    //win->ShowCursor(false);

    win->GetEventManager().Bind<MouseUpEvent>([win](MouseUpEvent &e) {  
        (void)win;
        std::cout << (int)e.GetMouseButtonType() << '\n'; 
    });

    /*win->GetEventManager().Bind<MouseMoveEvent>([win](MouseMoveEvent&e) {
        std::cout << e.GetPosition().x << "; " << e.GetPosition().y << '\n';
    });*/

    win->GetEventManager().Bind<MouseEnterEvent>(std::function<void(MouseEnterEvent&)>([](MouseEnterEvent&) {
        std::cout << "window entered" << '\n';
    }));

    ctrl->GetEventManager().Bind<MouseEnterEvent>(std::function<void(MouseEnterEvent&)>([](MouseEnterEvent&) {
        std::cout << "image control entered" << '\n';
    }));

    ctrl->GetEventManager().Bind<MouseExitEvent>(std::function<void(MouseExitEvent&)>([](MouseExitEvent&) {
         std::cout << "image control exited" << '\n';    
    }));

    win->GetEventManager().Bind<DrawEvent>([&position](DrawEvent &e){
        Painter painter(e);
        painter.Clear();
        painter.SetBrush(Brush(Color(100, 200, 100)));
        painter.DrawRectangle(Position(position, 60.0f), SizeF(100, 100));

    });

    win->EnableVSync(true);

    GameApp::GetGlobalApp()->Run();
}

TEST(ArxWindow, DISABLED_PositiveMouseInput2)
{
    ArxWindow *win = new ArxWindow("test", SizeF(640, 360));
    win->Show();
    win->SetFixedViewport(640, 360);

    std::filesystem::path testJpgPath(TEST_DATA_PATH / std::filesystem::path("test_png.png"));
    Image img(Image::LoadFromFile(testJpgPath.u8string()));
    ImageControl *ctrl = new ImageControl(win, img, SizeF(100, 100), Position(100, 100));
    ctrl->SetFilteringMode(Texture::TextureFilteringMode::Linear);
    ctrl->SetBackgroundColor(constants::COLOR_BLACK);

    ctrl->GetEventManager().Bind<MouseEnterEvent>(std::function<void(MouseEnterEvent&)>([](MouseEnterEvent&) {
        
        std::cout << "image control entered" << '\n';
    }));

    ctrl->GetEventManager().Bind<MouseExitEvent>(std::function<void(MouseExitEvent&)>([](MouseExitEvent&) {
        std::cout << "image control exited" << '\n';
    }));

    Timer *t = new Timer(win);
    t->GetEventManager().Bind<TimerEvent>([win](TimerEvent &){
        win->ShowCursor(false); 
    });

    t->SetInterval(std::chrono::seconds(5));
    t->Start(Timer::TimerType::SINGLE_FIRE);

    win->EnableVSync(true);
    GameApp::GetGlobalApp()->Run();
}


TEST(ArxWindow, DISABLED_PositiveLabel)
{
    ArxWindow *win = new ArxWindow("test", SizeF(640, 360));
    win->Show();
    win->SetFixedViewport(640, 360);
    Label *label = new Label(win);
    label->SetSize(SizeF(100, 100));
    label->SetText("Hello world");

    std::filesystem::path testFontPath(TEST_DATA_PATH / std::filesystem::path("test-font-roboto.ttf"));
    Font font(Font::LoadFromFile(testFontPath.u8string()));
    font.SetSizeInPt(30);
    ASSERT_FALSE(font.IsInvalid());
    label->SetFont(std::move(font));
    label->SetTextColor(Color("#666380"));
    Timer *t = new Timer(label);
    int counter = 0;
    t->GetEventManager().Bind<TimerEvent>([label, &counter] (TimerEvent &) mutable{
        label->SetText(std::to_string(counter++));
    });

    Label *label2 = new Label(win);
    label2->SetSize(SizeF(100, 100));
    label2->SetText("Hello world");
    label2->SetFont(label->GetFont());
    label2->SetTextColor(Color("#FF0000"));
    
    label->MoveToFront();
    ASSERT_NE(win->GetChildren().front(), label);

    t->SetInterval(std::chrono::seconds(1));
    t->Start(Timer::TimerType::CONTINUOUS);
    
    win->EnableVSync(true);
    GameApp::GetGlobalApp()->Run();
}

TEST(ArxWindow, PositiveBitmapButton)
{
    ArxWindow *win = new ArxWindow("test", SizeF(640, 360));
    win->Show();
    win->SetFixedViewport(640, 360);
    BitmapButton *btn = new BitmapButton(win);
    btn->SetSize(SizeF(100, 100));
    btn->SetPosition(Position(100, 100));
    btn->SetText("Hello world");
    btn->SetNormalImage(Image::LoadFromFile((TEST_DATA_PATH / std::filesystem::path("btn_normal.jpg")).u8string()));
    btn->SetMouseEnterImage(Image::LoadFromFile((TEST_DATA_PATH / std::filesystem::path("btn_hover.jpg")).u8string()));
    btn->SetMouseHoldImage(Image::LoadFromFile((TEST_DATA_PATH / std::filesystem::path("btn_hold.jpg")).u8string()));

    BitmapButton *btn3 = new BitmapButton(win);
    btn3->SetSize(SizeF(50, 50));
    btn3->SetPosition(Position(30, 10));
    btn3->SetText("Hello world");
    btn3->SetNormalImage(Image::LoadFromFile((TEST_DATA_PATH / std::filesystem::path("btn_hover.jpg")).u8string()));
    btn3->SetMouseEnterImage(Image::LoadFromFile((TEST_DATA_PATH / std::filesystem::path("btn_hold.jpg")).u8string()));
    btn3->SetMouseHoldImage(Image::LoadFromFile((TEST_DATA_PATH / std::filesystem::path("btn_normal.jpg")).u8string()));
    btn3->GetEventManager().Bind<MouseDownEvent>([btn](MouseDownEvent &e)
    {
        (void)btn;
        std::cout << "EnterBtn3" << '\n';
        e.Skip();
    });

    BitmapButton *btn2 = new BitmapButton(btn);
    btn2->SetSize(SizeF(50, 50));
    btn2->SetText("Hello world");
    btn2->SetNormalImage(Image::LoadFromFile((TEST_DATA_PATH / std::filesystem::path("btn_normal.jpg")).u8string()));
    btn2->SetMouseEnterImage(Image::LoadFromFile((TEST_DATA_PATH / std::filesystem::path("btn_hover.jpg")).u8string()));
    btn2->SetMouseHoldImage(Image::LoadFromFile((TEST_DATA_PATH / std::filesystem::path("btn_hold.jpg")).u8string()));

    std::filesystem::path testFontPath(TEST_DATA_PATH / std::filesystem::path("test-font-roboto.ttf"));
    Font font(Font::LoadFromFile(testFontPath.u8string()));
    font.SetSizeInPt(30);
    ASSERT_FALSE(font.IsInvalid());
    btn->SetFont(std::move(font));
    btn->SetText("test");
    btn2->GetEventManager().Bind<MouseEnterEvent>([btn](MouseEnterEvent &e)
    {
        (void)btn;
        std::cout << "EnterBtn2" << '\n';
        e.Skip();
    });

    btn2->GetEventManager().Bind<MouseExitEvent>([btn](MouseExitEvent &e)
    {
        (void)btn;
        std::cout << "ExitBtn2" << '\n';
        e.Skip();
    });

    btn->GetEventManager().Bind<MouseDownEvent>([btn](MouseDownEvent &e)
    {
        (void)btn;
        std::cout << "Click" << '\n';
        e.Skip();
    });

    win->EnableVSync(true);
    GameApp::GetGlobalApp()->Run();
}

