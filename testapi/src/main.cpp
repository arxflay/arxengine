#include <gtest/gtest.h>
#include "color.h"
#include <ArxWindow.h>
#include <UIApp.h>
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

int main(int argc, char **argv)
{
    std::unique_ptr<arx::UIApp> app(std::make_unique<arx::UIApp>());
    app->Init();
    arx::UIApp::SetInstance(app.release());
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}

TEST(Font, FontThings)
{
    FT_Library library;
    int error = FT_Init_FreeType(&library);
    ASSERT_EQ(error, 0);
    std::unique_ptr<std::remove_reference_t<decltype(*std::declval<FT_Library>())>, std::function<void(FT_Library)>> libraryGuard(library, FT_Done_FreeType);
    //-1 used to check num of faces
    FT_Face face;
    error = FT_New_Face(library, "/usr/share/fonts/TTF/Hack-Bold.ttf", 0, &face);
    if (error == FT_Err_Unknown_File_Format)
        std::cout << "Invalid file format" << '\n';
    else if (error != 0)
        std::cout << "Uknown error" << '\n';
    
    int glyphIndex = FT_Get_Char_Index(face, 0x1F028000u);
    std::cout << glyphIndex << '\n';
    
    FT_Done_Face(face);

    ASSERT_EQ(error, 0);
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
    //arx::ArxWindow *win3 = new arx::ArxWindow("3", arx::Size{200, 100}); 
    win->Show();
    //win3->Show();
    win->SetSize(arx::Size{100, 200});

    int val = arx::UIApp::GetInstance()->Exec();
    ASSERT_EQ(val, 0);
}
