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

#include "../src/OS/System.hpp"
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace Tiny::OS;

// ==================== FileSystem 类测试 ====================

// 测试 currentPath()
TEST(FileSystemTest, CurrentPath) {
    Path current = FileSystem::currentPath();
    EXPECT_TRUE(current.isValid());
    EXPECT_TRUE(current.isDirectory());
}

// 测试 homePath()
TEST(FileSystemTest, HomePath) {
    Path home = FileSystem::homePath();
    EXPECT_TRUE(home.isValid());
    EXPECT_TRUE(home.isDirectory());
}

// 测试 chDir() - 字符串参数
TEST(FileSystemTest, ChDir_String) {
    Path originalDir = FileSystem::currentPath();
    EXPECT_TRUE(FileSystem::chDir(".."));
    Path newDir = FileSystem::currentPath();
    EXPECT_TRUE(newDir.isValid());
    EXPECT_TRUE(FileSystem::chDir(originalDir.path()));
    Path backDir = FileSystem::currentPath();
    EXPECT_EQ(backDir.path(), originalDir.path());
}

// 测试 chDir() - Path参数
TEST(FileSystemTest, ChDir_Path) {
    Path originalDir = FileSystem::currentPath();
    Path parentDir("..");
    EXPECT_TRUE(FileSystem::chDir(parentDir));
    Path newDir = FileSystem::currentPath();
    EXPECT_TRUE(newDir.isValid());
    EXPECT_TRUE(FileSystem::chDir(originalDir));
    Path backDir = FileSystem::currentPath();
    EXPECT_EQ(backDir.path(), originalDir.path());
}

// 测试 chDir() - 无效路径
TEST(FileSystemTest, ChDir_Invalid) {
    EXPECT_FALSE(FileSystem::chDir("/this/path/does/not/exist/12345"));
}

// 测试 mkDir() 和 rmDir() - 字符串参数
TEST(FileSystemTest, MkDirAndRmDir_String) {
    std::string testDir = "./test_dir_temp";
    Path checkPath(testDir);
    if (checkPath.isValid()) FileSystem::rmDir(testDir, true);
    
    EXPECT_TRUE(FileSystem::mkDir(testDir));
    Path verifyPath(testDir);
    EXPECT_TRUE(verifyPath.isValid());
    EXPECT_TRUE(verifyPath.isDirectory());
    
    EXPECT_TRUE(FileSystem::rmDir(testDir));
    Path removedPath(testDir);
    EXPECT_FALSE(removedPath.isValid());
}

// 测试 mkDir() 和 rmDir() - Path参数
TEST(FileSystemTest, MkDirAndRmDir_Path) {
    std::string testDir = "./test_dir_temp_path";
    Path checkPath(testDir);
    if (checkPath.isValid()) FileSystem::rmDir(checkPath, true);
    
    Path dirPath(testDir);
    EXPECT_TRUE(FileSystem::mkDir(dirPath));
    Path verifyPath(testDir);
    EXPECT_TRUE(verifyPath.isValid());
    EXPECT_TRUE(verifyPath.isDirectory());
    
    Path removePath(testDir);
    EXPECT_TRUE(FileSystem::rmDir(removePath));
    Path removedPath(testDir);
    EXPECT_FALSE(removedPath.isValid());
}

// 测试 mkDir() - 已存在目录
TEST(FileSystemTest, MkDir_Existing) {
    std::string testDir = "./test_existing_dir";
    Path checkPath(testDir);
    if (checkPath.isValid()) FileSystem::rmDir(testDir, true);
    
    EXPECT_TRUE(FileSystem::mkDir(testDir));
    EXPECT_TRUE(FileSystem::mkDir(testDir));
    
    FileSystem::rmDir(testDir, true);
}

// 测试 rmDir() - 递归删除
TEST(FileSystemTest, RmDir_Recursion) {
    std::string parentDir = "./test_parent_dir";
    std::string childDir = parentDir + "/child_dir";
    std::string testFile = parentDir + "/test.txt";
    
    Path checkPath(parentDir);
    if (checkPath.isValid()) FileSystem::rmDir(parentDir, true);
    
    EXPECT_TRUE(FileSystem::mkDir(parentDir));
    EXPECT_TRUE(FileSystem::mkDir(childDir));
    EXPECT_TRUE(FileSystem::mkFile(testFile, "test content"));
    
    EXPECT_TRUE(FileSystem::rmDir(parentDir, true));
    Path removedPath(parentDir);
    EXPECT_FALSE(removedPath.isValid());
}

// 测试 mkFile() 和 rmFile() - 字符串参数，空文件
TEST(FileSystemTest, MkFileAndRmFile_String_Empty) {
    std::string testFile = "./test_file_empty.txt";
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    EXPECT_TRUE(FileSystem::mkFile(testFile));
    Path filePath(testFile);
    EXPECT_TRUE(filePath.isValid());
    EXPECT_TRUE(filePath.isFile());
    EXPECT_EQ(filePath.fileSize(), 0);
    
    EXPECT_TRUE(FileSystem::rmFile(testFile));
    Path removedPath(testFile);
    EXPECT_FALSE(removedPath.isValid());
}

// 测试 mkFile() 和 rmFile() - Path参数，空文件
TEST(FileSystemTest, MkFileAndRmFile_Path_Empty) {
    std::string testFile = "./test_file_empty_path.txt";
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(checkPath);
    
    Path filePath(testFile);
    EXPECT_TRUE(FileSystem::mkFile(filePath));
    Path verifyPath(testFile);
    EXPECT_TRUE(verifyPath.isValid());
    EXPECT_TRUE(verifyPath.isFile());
    
    Path removePath(testFile);
    EXPECT_TRUE(FileSystem::rmFile(removePath));
    Path removedPath(testFile);
    EXPECT_FALSE(removedPath.isValid());
}

// 测试 mkFile() - 字符串参数，带vector数据
TEST(FileSystemTest, MkFile_String_Vector) {
    std::string testFile = "./test_file_vector.bin";
    std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0x04, 0x05};
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    EXPECT_TRUE(FileSystem::mkFile(testFile, data));
    Path filePath(testFile);
    EXPECT_TRUE(filePath.isValid());
    EXPECT_TRUE(filePath.isFile());
    EXPECT_EQ(filePath.fileSize(), data.size());
    
    FileSystem::rmFile(testFile);
}

// 测试 mkFile() - Path参数，带vector数据
TEST(FileSystemTest, MkFile_Path_Vector) {
    std::string testFile = "./test_file_vector_path.bin";
    std::vector<uint8_t> data = {0x0A, 0x0B, 0x0C};
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(checkPath);
    
    Path filePath(testFile);
    EXPECT_TRUE(FileSystem::mkFile(filePath, data));
    Path verifyPath(testFile);
    EXPECT_TRUE(verifyPath.isValid());
    EXPECT_EQ(verifyPath.fileSize(), data.size());
    
    FileSystem::rmFile(filePath);
}

// 测试 mkFile() - 字符串参数，带string数据
TEST(FileSystemTest, MkFile_String_String) {
    std::string testFile = "./test_file_string.txt";
    std::string content = "Hello, World!";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    EXPECT_TRUE(FileSystem::mkFile(testFile, content));
    Path filePath(testFile);
    EXPECT_TRUE(filePath.isValid());
    EXPECT_TRUE(filePath.isFile());
    EXPECT_EQ(filePath.fileSize(), content.size());
    
    FileSystem::rmFile(testFile);
}

// 测试 mkFile() - Path参数，带string数据
TEST(FileSystemTest, MkFile_Path_String) {
    std::string testFile = "./test_file_string_path.txt";
    std::string content = "Test content with Path";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(checkPath);
    
    Path filePath(testFile);
    EXPECT_TRUE(FileSystem::mkFile(filePath, content));
    Path verifyPath(testFile);
    EXPECT_TRUE(verifyPath.isValid());
    EXPECT_EQ(verifyPath.fileSize(), content.size());
    
    FileSystem::rmFile(filePath);
}

// 测试 cpFile() - Path, Path
TEST(FileSystemTest, CpFile_PathPath) {
    std::string srcFile = "./test_cp_src.txt";
    std::string destFile = "./test_cp_dest.txt";
    std::string content = "Test content for copy";
    
    Path checkSrc(srcFile);
    if (checkSrc.isValid()) FileSystem::rmFile(srcFile);
    Path checkDest(destFile);
    if (checkDest.isValid()) FileSystem::rmFile(destFile);
    
    EXPECT_TRUE(FileSystem::mkFile(srcFile, content));
    
    Path src(srcFile);
    Path dest(destFile);
    EXPECT_TRUE(FileSystem::cpFile(src, dest));
    
    Path verifyDest(destFile);
    EXPECT_TRUE(verifyDest.isValid());
    EXPECT_TRUE(verifyDest.isFile());
    EXPECT_EQ(verifyDest.fileSize(), content.size());
    
    FileSystem::rmFile(srcFile);
    FileSystem::rmFile(destFile);
}

// 测试 cpFile() - Path, string
TEST(FileSystemTest, CpFile_PathString) {
    std::string srcFile = "./test_cp_src2.txt";
    std::string destFile = "./test_cp_dest2.txt";
    std::string content = "Test content for copy 2";
    
    Path checkSrc(srcFile);
    if (checkSrc.isValid()) FileSystem::rmFile(srcFile);
    Path checkDest(destFile);
    if (checkDest.isValid()) FileSystem::rmFile(destFile);
    
    EXPECT_TRUE(FileSystem::mkFile(srcFile, content));
    
    Path src(srcFile);
    EXPECT_TRUE(FileSystem::cpFile(src, destFile));
    
    Path verifyDest(destFile);
    EXPECT_TRUE(verifyDest.isValid());
    
    FileSystem::rmFile(srcFile);
    FileSystem::rmFile(destFile);
}

// 测试 mvFile() - Path, Path
TEST(FileSystemTest, MvFile_PathPath) {
    std::string srcFile = "./test_mv_src.txt";
    std::string destFile = "./test_mv_dest.txt";
    std::string content = "Test content for move";
    
    Path checkSrc(srcFile);
    if (checkSrc.isValid()) FileSystem::rmFile(srcFile);
    Path checkDest(destFile);
    if (checkDest.isValid()) FileSystem::rmFile(destFile);
    
    EXPECT_TRUE(FileSystem::mkFile(srcFile, content));
    
    Path src(srcFile);
    Path dest(destFile);
    FileSystem::mvFile(src, dest);
    
    if (Path(srcFile).isValid()) FileSystem::rmFile(srcFile);
    if (Path(destFile).isValid()) FileSystem::rmFile(destFile);
}

// 测试 mvFile() - Path, string
TEST(FileSystemTest, MvFile_PathString) {
    std::string srcFile = "./test_mv_src2.txt";
    std::string destFile = "./test_mv_dest2.txt";
    std::string content = "Test content for move 2";
    
    Path checkSrc(srcFile);
    if (checkSrc.isValid()) FileSystem::rmFile(srcFile);
    Path checkDest(destFile);
    if (checkDest.isValid()) FileSystem::rmFile(destFile);
    
    EXPECT_TRUE(FileSystem::mkFile(srcFile, content));
    
    Path src(srcFile);
    FileSystem::mvFile(src, destFile);
    
    if (Path(srcFile).isValid()) FileSystem::rmFile(srcFile);
    if (Path(destFile).isValid()) FileSystem::rmFile(destFile);
}

// 测试 cpDir() - Path, Path
TEST(FileSystemTest, CpDir_PathPath) {
    std::string srcDir = "./test_cpdir_src";
    std::string destDir = "./test_cpdir_dest";
    
    Path checkSrc(srcDir);
    if (checkSrc.isValid()) FileSystem::rmDir(srcDir, true);
    Path checkDest(destDir);
    if (checkDest.isValid()) FileSystem::rmDir(destDir, true);
    
    EXPECT_TRUE(FileSystem::mkDir(srcDir));
    EXPECT_TRUE(FileSystem::mkFile(srcDir + "/test.txt", "test content"));
    
    Path src(srcDir);
    Path dest(destDir);
    FileSystem::cpDir(src, dest);
    
    if (Path(srcDir).isValid()) FileSystem::rmDir(srcDir, true);
    if (Path(destDir).isValid()) FileSystem::rmDir(destDir, true);
}

// 测试 cpDir() - Path, string
TEST(FileSystemTest, CpDir_PathString) {
    std::string srcDir = "./test_cpdir_src2";
    std::string destDir = "./test_cpdir_dest2";
    
    Path checkSrc(srcDir);
    if (checkSrc.isValid()) FileSystem::rmDir(srcDir, true);
    Path checkDest(destDir);
    if (checkDest.isValid()) FileSystem::rmDir(destDir, true);
    
    EXPECT_TRUE(FileSystem::mkDir(srcDir));
    EXPECT_TRUE(FileSystem::mkFile(srcDir + "/test.txt", "test content"));
    
    Path src(srcDir);
    FileSystem::cpDir(src, destDir);
    
    if (Path(srcDir).isValid()) FileSystem::rmDir(srcDir, true);
    if (Path(destDir).isValid()) FileSystem::rmDir(destDir, true);
}

// 测试 mvDir() - Path, Path
TEST(FileSystemTest, MvDir_PathPath) {
    std::string srcDir = "./test_mvdir_src";
    std::string destDir = "./test_mvdir_dest";
    
    Path checkSrc(srcDir);
    if (checkSrc.isValid()) FileSystem::rmDir(srcDir, true);
    Path checkDest(destDir);
    if (checkDest.isValid()) FileSystem::rmDir(destDir, true);
    
    EXPECT_TRUE(FileSystem::mkDir(srcDir));
    
    Path src(srcDir);
    Path dest(destDir);
    FileSystem::mvDir(src, dest);
    
    if (Path(srcDir).isValid()) FileSystem::rmDir(srcDir, true);
    if (Path(destDir).isValid()) FileSystem::rmDir(destDir, true);
}

// 测试 mvDir() - Path, string
TEST(FileSystemTest, MvDir_PathString) {
    std::string srcDir = "./test_mvdir_src2";
    std::string destDir = "./test_mvdir_dest2";
    
    Path checkSrc(srcDir);
    if (checkSrc.isValid()) FileSystem::rmDir(srcDir, true);
    Path checkDest(destDir);
    if (checkDest.isValid()) FileSystem::rmDir(destDir, true);
    
    EXPECT_TRUE(FileSystem::mkDir(srcDir));
    
    Path src(srcDir);
    FileSystem::mvDir(src, destDir);
    
    if (Path(srcDir).isValid()) FileSystem::rmDir(srcDir, true);
    if (Path(destDir).isValid()) FileSystem::rmDir(destDir, true);
}

// 测试 mkLink() - string, string
TEST(FileSystemTest, MkLink_StringString) {
    std::string targetFile = "./test_link_target.txt";
    std::string linkFile = "./test_link.lnk";
    std::string content = "Test content for link";
    
    Path checkTarget(targetFile);
    if (checkTarget.isValid()) FileSystem::rmFile(targetFile);
    Path checkLink(linkFile);
    if (checkLink.isValid()) FileSystem::rmFile(linkFile);
    
    EXPECT_TRUE(FileSystem::mkFile(targetFile, content));
    FileSystem::mkLink(linkFile, targetFile);
    
    if (Path(targetFile).isValid()) FileSystem::rmFile(targetFile);
    if (Path(linkFile).isValid()) FileSystem::rmFile(linkFile);
}

// 测试 mkLink() - string, Path
TEST(FileSystemTest, MkLink_StringPath) {
    std::string targetFile = "./test_link_target2.txt";
    std::string linkFile = "./test_link2.lnk";
    std::string content = "Test content for link 2";
    
    Path checkTarget(targetFile);
    if (checkTarget.isValid()) FileSystem::rmFile(targetFile);
    Path checkLink(linkFile);
    if (checkLink.isValid()) FileSystem::rmFile(linkFile);
    
    EXPECT_TRUE(FileSystem::mkFile(targetFile, content));
    Path target(targetFile);
    FileSystem::mkLink(linkFile, target);
    
    if (Path(targetFile).isValid()) FileSystem::rmFile(targetFile);
    if (Path(linkFile).isValid()) FileSystem::rmFile(linkFile);
}

// 测试 listPath() - 字符串参数
TEST(FileSystemTest, ListPath_String) {
    std::vector<Path> paths = FileSystem::listPath(".", 1);
    EXPECT_FALSE(paths.empty());
}

// 测试 listPath() - Path参数
TEST(FileSystemTest, ListPath_Path) {
    Path currentDir(".");
    std::vector<Path> paths = FileSystem::listPath(currentDir, 1);
    EXPECT_FALSE(paths.empty());
}

// 测试 listPath() - 无参数
TEST(FileSystemTest, ListPath_NoArgs) {
    std::vector<Path> paths = FileSystem::listPath(1);
    EXPECT_FALSE(paths.empty());
}

// 测试 listPath() - 带过滤器
TEST(FileSystemTest, ListPath_WithFilter) {
    auto filter = [](const Path& p) { return p.isDirectory(); };
    std::vector<Path> paths = FileSystem::listPath(".", 1, filter);
    for (const auto& p : paths) {
        EXPECT_TRUE(p.isDirectory());
    }
}

// 测试 listPath() - 递归
TEST(FileSystemTest, ListPath_Recursion) {
    std::vector<Path> paths = FileSystem::listPath(".", 2);
    EXPECT_FALSE(paths.empty());
}

// ==================== 主函数 ====================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
