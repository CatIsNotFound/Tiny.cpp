#define TINY_CPP_OS_FILE_IMPL
#include "../src/OS/File.h"
#include <iostream>
#include <gtest/gtest.h>

TEST(FileTest, Path_test) {
    Tiny::OS::Path path(".");

    EXPECT_TRUE(path.isExist());
    EXPECT_TRUE(path.isDirectory());
    EXPECT_FALSE(path.isFile());
    EXPECT_EQ(path.fileType(), Tiny::OS::FileType::Directory);
    EXPECT_STRNE(path.path().data(), ".");
    path.setPath("./assets/File/test.txt");
    EXPECT_TRUE(path.isExist());
    EXPECT_FALSE(path.isDirectory());
    EXPECT_TRUE(path.isFile());
    EXPECT_EQ(path.fileType(), Tiny::OS::FileType::File);
    EXPECT_STRNE(path.path().data(), "./assets/File/test.txt");
    EXPECT_GT(path.fileSize(), 0);
}


