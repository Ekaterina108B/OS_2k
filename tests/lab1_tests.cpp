#include <gtest/gtest.h>
#include <fstream>
#include "parent.h"
#include "utils.h"

TEST(01, SimpleTests) {
    std::array<std::string, 8> input = {
            "file_for_child_1.txt",
            "file_for_child_2.txt"
            "",
            "Hello, World!",
            "123456",
            " I am MAI ",
            "abc!@#",
            "A"
    };
    std::ofstream file_name_1("file_for_child_1.txt");
    file_name_1.close();
    std::ofstream file_name_2("file_for_child_1.txt");
    file_name_2.close();

    ParentRoutine(getenv("PATH_TO_CHILD_1"), getenv("PATH_TO_CHILD_2"), stdin);


}

TEST(02, SimpleTests) {
    std::array<std::string, 4> input = {
            "  a ",
            "   Tab",
            "Long      long",
            "!@#$%^&*()_+"
    };
}