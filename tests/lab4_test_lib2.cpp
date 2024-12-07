#include <gtest/gtest.h>
#include "lib2.h"
#include <cstring>

TEST(Lib2Test_GCF, BasicTests) {
    EXPECT_EQ(GCF(12, 18), 6);
    EXPECT_EQ(GCF(54, 24), 6);
    EXPECT_EQ(GCF(7, 13), 1);
}

TEST(Lib2Test_GCF, BoundaryCases) {
    EXPECT_EQ(GCF(0, 5), 5);
    EXPECT_EQ(GCF(5, 0), 5);
    EXPECT_EQ(GCF(1, 1), 1);
}

TEST(Lib2Test_GCF, SpecialCases ) {
    EXPECT_EQ(GCF(16, 16), 16);
    EXPECT_EQ(GCF(48756, 3280), 4);
}

TEST(Lib2Test_Translation, BasicTests) {
    char* result = translation(10);
    EXPECT_STREQ(result, "101");
    delete[] result;
    
    result = translation(15);
    EXPECT_STREQ(result, "120");
    delete[] result;
}

TEST(Lib2Test_Translation, BoundaryCases) {    
    char* result = translation(0);
    EXPECT_STREQ(result, "0");
    delete[] result;
    
    result = translation(1);
    EXPECT_STREQ(result, "1");
    delete[] result;
}

TEST(Lib2Test_Translation, BigNumbers) {
    char* result = translation(27);
    EXPECT_STREQ(result, "1000");
    delete[] result;
    
    result = translation(100);
    EXPECT_STREQ(result, "10201");
    delete[] result;
} 