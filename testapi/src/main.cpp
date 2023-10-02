#include <gtest/gtest.h>
#include "color.h"
#include <ArxWindow.h>
#include <UIApp.h>

int main(void)
{
    std::unique_ptr<arx::UIApp> app(std::make_unique<arx::UIApp>());
    app->Init();
    arx::UIApp::SetInstance(app.release());
    ::testing::InitGoogleTest();
    int ret = RUN_ALL_TESTS();
    return ret;
}

TEST(Color, FromHexRGB)
{
    arx::Color c("#F0FF05");
    ASSERT_EQ(c.r, 255 - 0x0F);
    ASSERT_EQ(c.g, 255);
    ASSERT_EQ(c.b, 5);
}

TEST(Window, ShowWindow)
{
    arx::ArxWindow *win = new arx::ArxWindow("1", arx::Size{0, 0});
    arx::ArxWindow *win3 = new arx::ArxWindow("3", arx::Size{200, 100}); 
    win->Show();
    win3->Show();
    win->SetSize(arx::Size{100, 200});

    int val = arx::UIApp::GetInstance()->Exec();
    ASSERT_EQ(val, 0);
}
