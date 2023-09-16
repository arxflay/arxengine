#include <gtest/gtest.h>
#include "color.h"
#include <ArxEngine.h>
#include <ArxWindow.h>
#include <GLFW/glfw3.h>
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
    arx::ArxWindow *win = new arx::ArxWindow(nullptr, "1", arx::Size{100, 200});
    arx::ArxWindow *win2 = new arx::ArxWindow(win, "2", arx::Size{100, 200});
    arx::ArxWindow *win3 = new arx::ArxWindow(nullptr, "3", arx::Size{100, 200});
    
    win2->Show();
    win->Show();
    win3->Show();

    int val = arx::UIApp::GetInstance()->Exec();
    ASSERT_EQ(val, 0);
}
