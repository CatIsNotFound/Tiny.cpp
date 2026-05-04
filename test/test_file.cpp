#define TINY_CPP_OS_FILE_IMPL
#include "../src/OS/File.h"
#include <iostream>
#include <gtest/gtest.h>

TEST(FileTest, Path_test) {
    Tiny::OS::Path path(".");
    EXPECT_TRUE(path.isValid());
    EXPECT_TRUE(path.isDirectory());
    EXPECT_FALSE(path.isFile());
    EXPECT_EQ(path.fileType(), Tiny::OS::FileType::Directory);
    EXPECT_STRNE(path.path().data(), ".");
    std::cout << "File Path: " << path.path() << "\n";
    std::cout << "Short File name: " << path.shortFileName() << "\n";
    path.setPath("./assets/File/test.txt");
    EXPECT_TRUE(path.isValid());
    EXPECT_FALSE(path.isDirectory());
    EXPECT_TRUE(path.isFile());
    EXPECT_EQ(path.fileType(), Tiny::OS::FileType::File);
    EXPECT_STRNE(path.path().data(), "./assets/File/test.txt");
    EXPECT_GT(path.fileSize(), 0);
}

TEST(FileTest, View_directory) {
    auto current_path = Tiny::OS::FileSystem::currentPath();
    auto old_path_name = current_path.path();
    std::cout << "Current path: " << current_path.path() << "\n";
    auto path_list = Tiny::OS::FileSystem::listPath(old_path_name, 0);
    EXPECT_FALSE(path_list.empty());
    for (auto& path : path_list) {
        std::cout << "Path: " << path.path() << "\n";
    }
    EXPECT_TRUE(Tiny::OS::FileSystem::chDir(Tiny::OS::FileSystem::homePath()));
    current_path.setPath(Tiny::OS::FileSystem::currentPath());
    std::cout << "New path: " << current_path.path() << "(" << current_path.path().size() << ")" << "\n";
    std::vector<Tiny::OS::Path> path_result = Tiny::OS::FileSystem::listPath(4);

    for (auto& path : path_result) {
        if (path.isValid())
        std::cout << Tiny::OS::fileTypeName(path.fileType()) << ": " << path.path() << " - " << path.fileSize() << "\n";
    }
    EXPECT_GT(path_result.size(), 0);
    EXPECT_TRUE(Tiny::OS::FileSystem::chDir(old_path_name));
}


TEST(FileTest, File_Operate) {
    auto current_path = Tiny::OS::FileSystem::currentPath();
    auto temp_path = Tiny::OS::Path(current_path.path() + "/test_dir");
    if (temp_path.isValid()) {
        EXPECT_TRUE(Tiny::OS::FileSystem::rmDir(temp_path, true));
    }
    EXPECT_TRUE(Tiny::OS::FileSystem::makeDirectory(temp_path));
    temp_path.setPath("./test_dir/test.txt");
    EXPECT_TRUE(Tiny::OS::FileSystem::makeFile(temp_path, "Here is a sample text!"));
    EXPECT_TRUE(Tiny::OS::FileSystem::chDir("./test_dir"));
    EXPECT_FALSE(Tiny::OS::FileSystem::chDir("./test_dir/test.txt"));

}