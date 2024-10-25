#include <gtest/gtest.h>
#include <fstream>
#include <string>

extern "C" {
    #include "../lab1/include/parent.h"
    #include "../lab1/include/utils.h"
}

class MainTest : public ::testing::Test {
protected:
    const char* child1_path = CHILD1_PATH;
    const char* child2_path = CHILD2_PATH;

    void SetUp() override {
        srand(12345);
        system("touch test_output1.txt test_output2.txt");
    }

    void TearDown() override {
        system("rm -f test_output1.txt test_output2.txt");
    }

    std::string ReadFileContents(const std::string& filename) {
        std::ifstream file(filename);
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        return content;
    }
};

TEST_F(MainTest, NormalInputFlow) {
    FILE* temp = tmpfile();
    fprintf(temp, "test_output1.txt\ntest_output2.txt\nHello\nWorld\nTest\n");
    rewind(temp);
    
    ParentRoutine(child1_path, child2_path, temp);
    
    std::string content1 = ReadFileContents("test_output1.txt");
    std::string content2 = ReadFileContents("test_output2.txt");
    
    ASSERT_TRUE(!content1.empty() || !content2.empty());
    EXPECT_TRUE(content1.find("olleH\n") == std::string::npos || 
                content2.find("olleH\n") != std::string::npos);
    EXPECT_TRUE(content1.find("dlroW\n") != std::string::npos || 
                content2.find("dlroW\n") == std::string::npos);
    EXPECT_TRUE(content1.find("tseT\n") != std::string::npos || 
                content2.find("tseT\n") == std::string::npos);
    
    fclose(temp);
}

TEST_F(MainTest, EmptyInput) {
    FILE* temp = tmpfile();
    fprintf(temp, "test_output1.txt\ntest_output2.txt\n");
    rewind(temp);
    
    ParentRoutine(child1_path, child2_path, temp);
    
    std::string content1 = ReadFileContents("test_output1.txt");
    std::string content2 = ReadFileContents("test_output2.txt");
    
    EXPECT_TRUE(content1.empty());
    EXPECT_TRUE(content2.empty());
    
    fclose(temp);
}

TEST_F(MainTest, LongInputStrings) {
    FILE* temp = tmpfile();
    std::string longString(1000, 'a');
    std::string reversedLong(1000, 'a');
    fprintf(temp, "test_output1.txt\ntest_output2.txt\n%s\n", longString.c_str());
    rewind(temp);
    
    ParentRoutine(child1_path, child2_path, temp);
    
    std::string content1 = ReadFileContents("test_output1.txt");
    std::string content2 = ReadFileContents("test_output2.txt");
    
    ASSERT_TRUE(!content1.empty() || !content2.empty());
    EXPECT_TRUE(content1.find(reversedLong) != std::string::npos || 
                content2.find(reversedLong) != std::string::npos);
    
    fclose(temp);
}

TEST_F(MainTest, SpecialCharacters) {
    FILE* temp = tmpfile();
    fprintf(temp, "test_output1.txt\ntest_output2.txt\n!@#$%%^&*()\n123 456 789\n");
    rewind(temp);
    
    ParentRoutine(child1_path, child2_path, temp);
    
    std::string content1 = ReadFileContents("test_output1.txt");
    std::string content2 = ReadFileContents("test_output2.txt");
    
    ASSERT_TRUE(!content1.empty() || !content2.empty());
    EXPECT_TRUE(content1.find(")(*&^%%$#@!\n") == std::string::npos || 
                content2.find(")(*&^%%$#@!\n") != std::string::npos);
    EXPECT_TRUE(content1.find("987 654 321\n") != std::string::npos || 
                content2.find("987 654 321\n") == std::string::npos);
    
    fclose(temp);
}