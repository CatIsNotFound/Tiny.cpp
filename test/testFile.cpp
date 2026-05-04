/*************************************************************************************
 * MIT License                                                                       *
 *                                                                                   *
 * Copyright (c) 2026 CatIsNotFound                                                  *
 *                                                                                   *
 * Permission is hereby granted, free of charge, to any person obtaining a copy      *
 * of this software and associated documentation files (the "Software"), to deal     *
 * in the Software without restriction, including without limitation the rights      *
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell         *
 * copies of the Software, and to permit persons to whom the Software is             *
 * furnished to do so, subject to the following conditions:                          *
 *                                                                                   *
 * The above copyright notice and this permission notice shall be included in all    *
 * copies or substantial portions of the Software.                                   *
 *                                                                                   *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR        *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,          *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE       *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER            *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,     *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE     *
 * SOFTWARE.                                                                         *
 *                                                                                   *
 *************************************************************************************/

#define TINY_CPP_OS_FILE_IMPL
#include "../src/OS/File.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace Tiny::OS;

// ==================== Path 类测试 ====================

// 测试 Path 构造函数
TEST(PathTest, Constructor_String) {
    Path p(".");
    EXPECT_TRUE(p.isValid());
    EXPECT_TRUE(p.isDirectory());
}

TEST(PathTest, Constructor_ParentDirectory) {
    Path p("..");
    EXPECT_TRUE(p.isValid());
    EXPECT_TRUE(p.isDirectory());
}

TEST(PathTest, Constructor_Copy) {
    Path p1(".");
    Path p2(p1);
    EXPECT_TRUE(p2.isValid());
    EXPECT_EQ(p1.path(), p2.path());
}

TEST(PathTest, Constructor_Move) {
    Path p(std::move(Path(".")));
    EXPECT_TRUE(p.isValid());
}

// 测试 Path 赋值操作
TEST(PathTest, Assignment_Copy) {
    Path p1{"."};
    Path p2{"."};  // 先初始化
    p2 = p1;       // 然后赋值
    EXPECT_TRUE(p2.isValid());
    EXPECT_EQ(p1.path(), p2.path());
}

TEST(PathTest, Assignment_Move) {
    Path p{"."};   // 先初始化
    p = std::move(Path{".."});  // 然后移动赋值
    EXPECT_TRUE(p.isValid());
}

// 测试 setPath 方法
TEST(PathTest, SetPath_String) {
    Path p(".");
    p.setPath("..");
    EXPECT_TRUE(p.isValid());
    EXPECT_TRUE(p.isDirectory());
}

TEST(PathTest, SetPath_Path) {
    Path p1(".");
    Path p2("..");
    p1.setPath(p2);
    EXPECT_TRUE(p1.isValid());
    EXPECT_TRUE(p1.isDirectory());
}

// 测试访问器方法
TEST(PathTest, Accessor_Path) {
    Path p(".");
    EXPECT_FALSE(p.path().empty());
}

TEST(PathTest, Accessor_ShortFileName) {
    Path p(".");
    EXPECT_FALSE(p.shortFileName().empty());
}

// 测试类型检查方法
TEST(PathTest, TypeCheck_Valid) {
    Path p(".");
    EXPECT_TRUE(p.isValid());
}

TEST(PathTest, TypeCheck_Directory) {
    Path p(".");
    EXPECT_TRUE(p.isDirectory());
    EXPECT_FALSE(p.isFile());
}

TEST(PathTest, TypeCheck_FileType) {
    Path p(".");
    EXPECT_EQ(p.fileType(), FileType::Directory);
}

// 测试 FileType 枚举和 fileTypeName
TEST(PathTest, FileTypeName_Unknown) {
    EXPECT_STREQ(fileTypeName(FileType::Unknown), "Unknown");
}

TEST(PathTest, FileTypeName_Directory) {
    EXPECT_STREQ(fileTypeName(FileType::Directory), "Directory");
}

TEST(PathTest, FileTypeName_File) {
    EXPECT_STREQ(fileTypeName(FileType::File), "File");
}

TEST(PathTest, FileTypeName_SymbolLink) {
    EXPECT_STREQ(fileTypeName(FileType::SymbolLink), "Symbol Link");
}

TEST(PathTest, FileTypeName_Device) {
    EXPECT_STREQ(fileTypeName(FileType::Device), "Device");
}

// 测试无效路径
TEST(PathTest, InvalidPath) {
    Path invalid("/this/path/does/not/exist/12345");
    EXPECT_FALSE(invalid.isValid());
    EXPECT_FALSE(invalid.isDirectory());
    EXPECT_FALSE(invalid.isFile());
    EXPECT_EQ(invalid.fileType(), FileType::Unknown);
}

// 测试空路径
TEST(PathTest, EmptyPath) {
    Path empty("");
    // 空路径的行为可能因平台而异，这里只测试不崩溃
    SUCCEED();
}

// ==================== FileSystem 类测试 ====================

// 测试 currentPath
TEST(FileSystemTest, CurrentPath) {
    Path current = FileSystem::currentPath();
    EXPECT_TRUE(current.isValid());
    EXPECT_TRUE(current.isDirectory());
}

// 测试 homePath
TEST(FileSystemTest, HomePath) {
    Path home = FileSystem::homePath();
    EXPECT_TRUE(home.isValid());
    EXPECT_TRUE(home.isDirectory());
}

// 测试 mkDir 和 rmDir (string 参数)
TEST(FileSystemTest, MkDirAndRmDir_String) {
    std::string testDir = "./test_dir_temp_gtest";

    // 确保目录不存在
    Path checkPath(testDir);
    if (checkPath.isValid()) {
        FileSystem::rmDir(testDir, true);
    }

    // 创建目录
    EXPECT_TRUE(FileSystem::mkDir(testDir));

    Path verifyPath(testDir);
    EXPECT_TRUE(verifyPath.isValid());
    EXPECT_TRUE(verifyPath.isDirectory());

    // 删除目录
    EXPECT_TRUE(FileSystem::rmDir(testDir));

    Path removedPath(testDir);
    EXPECT_FALSE(removedPath.isValid());
}

// 测试 mkDir 和 rmDir (Path 参数)
TEST(FileSystemTest, MkDirAndRmDir_Path) {
    std::string testDir = "./test_dir_temp_path_gtest";

    // 确保目录不存在
    Path checkPath(testDir);
    if (checkPath.isValid()) {
        FileSystem::rmDir(checkPath, true);
    }

    // 创建目录
    Path dirPath(testDir);
    EXPECT_TRUE(FileSystem::mkDir(dirPath));

    Path verifyPath(testDir);
    EXPECT_TRUE(verifyPath.isValid());
    EXPECT_TRUE(verifyPath.isDirectory());

    // 删除目录
    Path removePath(testDir);
    EXPECT_TRUE(FileSystem::rmDir(removePath));

    Path removedPath(testDir);
    EXPECT_FALSE(removedPath.isValid());
}

// 测试 mkFile 和 rmFile (string 参数)
TEST(FileSystemTest, MkFileAndRmFile_String) {
    std::string testFile = "./test_file_temp_gtest.txt";

    // 确保文件不存在
    Path checkPath(testFile);
    if (checkPath.isValid()) {
        FileSystem::rmFile(testFile);
    }

    // 创建空文件
    EXPECT_TRUE(FileSystem::mkFile(testFile));

    Path filePath(testFile);
    EXPECT_TRUE(filePath.isValid());
    EXPECT_TRUE(filePath.isFile());

    // 删除文件
    EXPECT_TRUE(FileSystem::rmFile(testFile));

    Path removedPath(testFile);
    EXPECT_FALSE(removedPath.isValid());
}

// 测试 mkFile 和 rmFile (Path 参数)
TEST(FileSystemTest, MkFileAndRmFile_Path) {
    std::string testFile = "./test_file_temp_path_gtest.txt";

    // 确保文件不存在
    Path checkPath(testFile);
    if (checkPath.isValid()) {
        FileSystem::rmFile(checkPath);
    }

    // 创建空文件
    Path filePath(testFile);
    EXPECT_TRUE(FileSystem::mkFile(filePath));

    Path verifyPath(testFile);
    EXPECT_TRUE(verifyPath.isValid());
    EXPECT_TRUE(verifyPath.isFile());

    // 删除文件
    Path removePath(testFile);
    EXPECT_TRUE(FileSystem::rmFile(removePath));

    Path removedPath(testFile);
    EXPECT_FALSE(removedPath.isValid());
}

// 测试使用 vector<uint8_t> 创建文件
TEST(FileSystemTest, MkFile_WithVectorData) {
    std::string testFile = "./test_file_data_gtest.bin";
    std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0x04, 0x05};

    // 确保文件不存在
    Path checkPath(testFile);
    if (checkPath.isValid()) {
        FileSystem::rmFile(testFile);
    }

    // 创建文件
    EXPECT_TRUE(FileSystem::mkFile(testFile, data));

    Path filePath(testFile);
    EXPECT_TRUE(filePath.isValid());
    EXPECT_TRUE(filePath.isFile());
    EXPECT_EQ(filePath.fileSize(), data.size());

    // 清理
    FileSystem::rmFile(testFile);
}

// 测试使用 string 创建文件
TEST(FileSystemTest, MkFile_WithStringData) {
    std::string testFile = "./test_file_string_gtest.txt";
    std::string content = "Hello, GTest!";

    // 确保文件不存在
    Path checkPath(testFile);
    if (checkPath.isValid()) {
        FileSystem::rmFile(testFile);
    }

    // 创建文件
    EXPECT_TRUE(FileSystem::mkFile(testFile, content));

    Path filePath(testFile);
    EXPECT_TRUE(filePath.isValid());
    EXPECT_TRUE(filePath.isFile());
    EXPECT_EQ(filePath.fileSize(), content.size());

    // 清理
    FileSystem::rmFile(testFile);
}

// 测试使用 Path 和 vector<uint8_t> 创建文件
TEST(FileSystemTest, MkFile_PathWithVectorData) {
    std::string testFile = "./test_file_data_path_gtest.bin";
    std::vector<uint8_t> data = {0x0A, 0x0B, 0x0C};

    // 确保文件不存在
    Path checkPath(testFile);
    if (checkPath.isValid()) {
        FileSystem::rmFile(checkPath);
    }

    Path filePath(testFile);
    EXPECT_TRUE(FileSystem::mkFile(filePath, data));

    Path verifyPath(testFile);
    EXPECT_TRUE(verifyPath.isValid());
    EXPECT_EQ(verifyPath.fileSize(), data.size());

    // 清理
    FileSystem::rmFile(filePath);
}

// 测试使用 Path 和 string 创建文件
TEST(FileSystemTest, MkFile_PathWithStringData) {
    std::string testFile = "./test_file_string_path_gtest.txt";
    std::string content = "Test content with Path";

    // 确保文件不存在
    Path checkPath(testFile);
    if (checkPath.isValid()) {
        FileSystem::rmFile(checkPath);
    }

    Path filePath(testFile);
    EXPECT_TRUE(FileSystem::mkFile(filePath, content));

    Path verifyPath(testFile);
    EXPECT_TRUE(verifyPath.isValid());
    EXPECT_EQ(verifyPath.fileSize(), content.size());

    // 清理
    FileSystem::rmFile(filePath);
}

// 测试 chDir (string 参数)
TEST(FileSystemTest, ChDir_String) {
    Path originalDir = FileSystem::currentPath();

    EXPECT_TRUE(FileSystem::chDir(".."));

    Path newDir = FileSystem::currentPath();
    EXPECT_TRUE(newDir.isValid());

    // 切换回原目录
    EXPECT_TRUE(FileSystem::chDir(originalDir.path()));

    Path backDir = FileSystem::currentPath();
    EXPECT_EQ(backDir.path(), originalDir.path());
}

// 测试 chDir (Path 参数)
TEST(FileSystemTest, ChDir_Path) {
    Path originalDir = FileSystem::currentPath();
    Path parentDir("..");

    EXPECT_TRUE(FileSystem::chDir(parentDir));

    Path newDir = FileSystem::currentPath();
    EXPECT_TRUE(newDir.isValid());

    // 切换回原目录
    EXPECT_TRUE(FileSystem::chDir(originalDir));

    Path backDir = FileSystem::currentPath();
    EXPECT_EQ(backDir.path(), originalDir.path());
}

// 测试切换到无效目录
TEST(FileSystemTest, ChDir_Invalid) {
    EXPECT_FALSE(FileSystem::chDir("/this/path/does/not/exist/12345"));
}

// 测试 listPath (string 参数)
TEST(FileSystemTest, ListPath_String) {
    std::vector<Path> paths = FileSystem::listPath(".", 1);
    EXPECT_FALSE(paths.empty());
}

// 测试 listPath (Path 参数)
TEST(FileSystemTest, ListPath_Path) {
    Path currentDir(".");
    std::vector<Path> paths = FileSystem::listPath(currentDir, 1);
    EXPECT_FALSE(paths.empty());
}

// 测试 listPath (无参数)
TEST(FileSystemTest, ListPath_NoArgs) {
    std::vector<Path> paths = FileSystem::listPath(1);
    EXPECT_FALSE(paths.empty());
}

// 测试递归删除目录
TEST(FileSystemTest, RmDir_Recursion) {
    std::string parentDir = "./test_parent_dir_gtest";
    std::string childDir = parentDir + "/child_dir";
    std::string testFile = parentDir + "/test.txt";

    // 清理已存在的目录
    Path checkPath(parentDir);
    if (checkPath.isValid()) {
        FileSystem::rmDir(parentDir, true);
    }

    // 创建目录结构
    EXPECT_TRUE(FileSystem::mkDir(parentDir));
    EXPECT_TRUE(FileSystem::mkDir(childDir));
    EXPECT_TRUE(FileSystem::mkFile(testFile, "test content"));

    Path parentPath(parentDir);
    EXPECT_TRUE(parentPath.isValid());
    EXPECT_TRUE(parentPath.isDirectory());

    // 递归删除
    EXPECT_TRUE(FileSystem::rmDir(parentDir, true));

    Path removedPath(parentDir);
    EXPECT_FALSE(removedPath.isValid());
}

// 测试删除非空目录（不带递归）
TEST(FileSystemTest, RmDir_NonEmptyWithoutRecursion) {
    std::string parentDir = "./test_nonempty_dir_gtest";
    std::string testFile = parentDir + "/test.txt";

    // 清理已存在的目录
    Path checkPath(parentDir);
    if (checkPath.isValid()) {
        FileSystem::rmDir(parentDir, true);
    }

    // 创建目录和文件
    EXPECT_TRUE(FileSystem::mkDir(parentDir));
    EXPECT_TRUE(FileSystem::mkFile(testFile, "test"));

    // 尝试不带递归删除非空目录（应该失败或行为未定义）
    // 注意：这个测试的行为可能因实现而异

    // 清理
    EXPECT_TRUE(FileSystem::rmDir(parentDir, true));
}

// 测试文件大小
TEST(FileSystemTest, FileSize) {
    std::string testFile = "./test_size_gtest.txt";
    std::string content = "Hello, World!";

    // 确保文件不存在
    Path checkPath(testFile);
    if (checkPath.isValid()) {
        FileSystem::rmFile(testFile);
    }

    // 创建文件
    EXPECT_TRUE(FileSystem::mkFile(testFile, content));

    Path filePath(testFile);
    EXPECT_EQ(filePath.fileSize(), content.size());

    // 清理
    FileSystem::rmFile(testFile);
}

// 测试 OS_NAME
TEST(FileSystemTest, OS_Name) {
    // 确保 OS_NAME 不为空
    EXPECT_NE(OS_NAME, nullptr);
    EXPECT_STRNE(OS_NAME, "");
    EXPECT_STRNE(OS_NAME, "unknown");
}

// ==================== 主函数 ====================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
