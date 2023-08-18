#include <gtest/gtest.h>
#include "color.h"

int main(void)
{
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}

TEST(Color, FromHexRGB)
{
    arx::Color c("#F0FF05");
    ASSERT_EQ(c.r, 255 - 0x0F);
    ASSERT_EQ(c.g, 255);
    ASSERT_EQ(c.b, 5);
}
