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

#include "../src/OS/File.hpp"
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <type_traits>

using namespace Tiny::OS;

// ==================== Path 类测试 ====================

// 测试 Path 构造函数 - 字符串
TEST(PathTest, Constructor_String) {
    Path p(".");
    EXPECT_TRUE(p.isValid());
    EXPECT_TRUE(p.isDirectory());
}

// 测试 Path 构造函数 - 父目录
TEST(PathTest, Constructor_ParentDirectory) {
    Path p("..");
    EXPECT_TRUE(p.isValid());
    EXPECT_TRUE(p.isDirectory());
}

// 测试 Path 构造函数 - 复制构造
TEST(PathTest, Constructor_Copy) {
    Path p1(".");
    Path p2(p1);
    EXPECT_TRUE(p2.isValid());
    EXPECT_EQ(p1.path(), p2.path());
}

// 测试 Path 构造函数 - 移动构造
TEST(PathTest, Constructor_Move) {
    Path p(std::move(Path(".")));
    EXPECT_TRUE(p.isValid());
}

// 测试 Path 赋值操作 - 复制赋值
TEST(PathTest, Assignment_Copy) {
    Path p1{"."};
    Path p2{".."};
    p2 = p1;
    EXPECT_TRUE(p2.isValid());
    EXPECT_EQ(p1.path(), p2.path());
}

// 测试 Path 赋值操作 - 移动赋值
TEST(PathTest, Assignment_Move) {
    Path p{"."};
    p = std::move(Path{".."});
    EXPECT_TRUE(p.isValid());
}

// 测试 setPath 方法 - 字符串参数
TEST(PathTest, SetPath_String) {
    Path p(".");
    p.setPath("..");
    EXPECT_TRUE(p.isValid());
    EXPECT_TRUE(p.isDirectory());
}

// 测试 setPath 方法 - Path参数
TEST(PathTest, SetPath_Path) {
    Path p1(".");
    Path p2("..");
    p1.setPath(p2);
    EXPECT_TRUE(p1.isValid());
    EXPECT_TRUE(p1.isDirectory());
}

// 测试 path() 访问器
TEST(PathTest, Accessor_Path) {
    Path p(".");
    EXPECT_FALSE(p.path().empty());
}

// 测试 shortFileName() 访问器
TEST(PathTest, Accessor_ShortFileName) {
    Path p(".");
    EXPECT_FALSE(p.shortFileName().empty());
}

// 测试 extensionName() 方法 - 文件
TEST(PathTest, Accessor_ExtensionName_File) {
    std::string testFile = "./test_extension.txt";
    FileSystem::mkFile(testFile);
    
    Path p(testFile);
    EXPECT_EQ(p.extensionName(), "txt");
    
    FileSystem::rmFile(testFile);
}

// 测试 extensionName() 方法 - 目录
TEST(PathTest, Accessor_ExtensionName_Directory) {
    Path p(".");
    EXPECT_TRUE(p.extensionName().empty());
}

// 测试 extensionName() 方法 - 无扩展名文件
TEST(PathTest, Accessor_ExtensionName_NoExtension) {
    std::string testFile = "./test_no_extension";
    FileSystem::mkFile(testFile);
    
    Path p(testFile);
    EXPECT_TRUE(p.extensionName().empty());
    
    FileSystem::rmFile(testFile);
}

// 测试 fileNameWithoutExtension() 方法 - 文件
TEST(PathTest, Accessor_FileNameWithoutExtension_File) {
    std::string testFile = "./test_filename.txt";
    FileSystem::mkFile(testFile);
    
    Path p(testFile);
    std::string name = p.fileNameWithoutExtension();
    EXPECT_FALSE(name.empty());
    
    FileSystem::rmFile(testFile);
}

// 测试 fileNameWithoutExtension() 方法 - 目录
TEST(PathTest, Accessor_FileNameWithoutExtension_Directory) {
    Path p(".");
    EXPECT_EQ(p.fileNameWithoutExtension(), p.shortFileName());
}

// 测试 parentDirectory() 方法
TEST(PathTest, Accessor_ParentDirectory) {
    Path p("./test_dir/test.txt");
    std::string parent = p.parentDirectory();
    EXPECT_FALSE(parent.empty());
}

// 测试 parentDirectory() 方法 - 根目录
TEST(PathTest, Accessor_ParentDirectory_Root) {
#if defined(_WIN32) || defined(_WIN64)
    Path p("C:\\");
    std::string parent = p.parentDirectory();
    EXPECT_FALSE(parent.empty());
#else
    Path p("/");
    std::string parent = p.parentDirectory();
    EXPECT_EQ(parent, "/");
#endif
}

// 测试 isValid() 方法
TEST(PathTest, TypeCheck_Valid) {
    Path p(".");
    EXPECT_TRUE(p.isValid());
}

// 测试 isDirectory() 方法
TEST(PathTest, TypeCheck_Directory) {
    Path p(".");
    EXPECT_TRUE(p.isDirectory());
    EXPECT_FALSE(p.isFile());
}

// 测试 isFile() 方法
TEST(PathTest, TypeCheck_File) {
    std::string testFile = "./test_isfile.txt";
    FileSystem::mkFile(testFile);
    
    Path p(testFile);
    EXPECT_TRUE(p.isFile());
    EXPECT_FALSE(p.isDirectory());
    
    FileSystem::rmFile(testFile);
}

// 测试 isSymbolLink() 方法
TEST(PathTest, TypeCheck_IsSymbolLink) {
    Path p(".");
    EXPECT_FALSE(p.isSymbolLink());
}

// 测试 fileType() 方法
TEST(PathTest, TypeCheck_FileType) {
    Path p(".");
    EXPECT_EQ(p.fileType(), FileType::Directory);
}

// 测试 fileSize() 方法
TEST(PathTest, Accessor_FileSize) {
    std::string testFile = "./test_size.txt";
    std::string content = "Hello, World!";
    FileSystem::mkFile(testFile, content);
    
    Path p(testFile);
    EXPECT_EQ(p.fileSize(), content.size());
    
    FileSystem::rmFile(testFile);
}

// 测试 operator/ 方法
TEST(PathTest, Operator_Divide) {
    Path p(".");
    Path& result = p / "subdir";
    EXPECT_EQ(&result, &p);
    EXPECT_FALSE(p.path().empty());
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
    SUCCEED();
}

// ==================== FileType 枚举测试 ====================

TEST(FileTypeTest, FileTypeName_Unknown) {
    EXPECT_STREQ(fileTypeName(FileType::Unknown), "Unknown");
}

TEST(FileTypeTest, FileTypeName_Directory) {
    EXPECT_STREQ(fileTypeName(FileType::Directory), "Directory");
}

TEST(FileTypeTest, FileTypeName_File) {
    EXPECT_STREQ(fileTypeName(FileType::File), "File");
}

TEST(FileTypeTest, FileTypeName_SymbolLink) {
    EXPECT_STREQ(fileTypeName(FileType::SymbolLink), "Symbol Link");
}

TEST(FileTypeTest, FileTypeName_Device) {
    EXPECT_STREQ(fileTypeName(FileType::Device), "Device");
}

TEST(FileTypeTest, FileTypeName_Socket) {
    EXPECT_STREQ(fileTypeName(FileType::Socket), "Socket");
}

// ==================== OS_NAME 测试 ====================

TEST(OSTest, OS_Name) {
    EXPECT_NE(OS_NAME, nullptr);
    EXPECT_STRNE(OS_NAME, "");
    EXPECT_STRNE(OS_NAME, "unknown");
}

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

// ==================== File 类测试 ====================

// File 类是 move-only 类型，不能拷贝，只能移动
// 移动后源对象会被重置为未打开状态

// 测试 File 类型特性 - 拷贝构造/赋值被删除
TEST(FileTest, TypeTrait_NoCopy) {
    EXPECT_FALSE(std::is_copy_constructible_v<File>);
    EXPECT_FALSE(std::is_copy_assignable_v<File>);
}

// 测试 File 类型特性 - 移动构造/赋值可用
TEST(FileTest, TypeTrait_MoveOnly) {
    EXPECT_TRUE(std::is_move_constructible_v<File>);
    EXPECT_TRUE(std::is_move_assignable_v<File>);
}

// 测试 File 构造函数 - 字符串路径 + ReadOnly
TEST(FileTest, Constructor_String_ReadOnly) {
    std::string testFile = "./test_file_ctor_str_r.txt";
    std::string content = "Test content";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, content);
    
    File file(testFile, ReadOnly);
    EXPECT_TRUE(file.isValid());
    EXPECT_TRUE(file.isOpen());
    
    file.close();
    FileSystem::rmFile(testFile);
}

// 测试 File 构造函数 - 字符串路径 + WriteOnly
TEST(FileTest, Constructor_String_WriteOnly) {
    std::string testFile = "./test_file_ctor_str_w.txt";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile);
    
    File file(testFile, WriteOnly);
    EXPECT_TRUE(file.isValid());
    EXPECT_TRUE(file.isOpen());
    
    file.close();
    FileSystem::rmFile(testFile);
}

// 测试 File 构造函数 - 字符串路径 + ReadWrite
TEST(FileTest, Constructor_String_ReadWrite) {
    std::string testFile = "./test_file_ctor_str_rw.txt";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile);
    
    File file(testFile, ReadWrite);
    EXPECT_TRUE(file.isValid());
    EXPECT_TRUE(file.isOpen());
    
    file.close();
    FileSystem::rmFile(testFile);
}

// 测试 File 构造函数 - 字符串路径 + Unknown（不打开）
TEST(FileTest, Constructor_String_Unknown) {
    std::string testFile = "./test_file_ctor_str_u.txt";
    std::string content = "Test content";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, content);
    
    File file(testFile, Unknown);
    EXPECT_TRUE(file.isValid());
    EXPECT_FALSE(file.isOpen());
    
    FileSystem::rmFile(testFile);
}

// 测试 File 构造函数 - Path对象 + ReadOnly
TEST(FileTest, Constructor_Path_ReadOnly) {
    std::string testFile = "./test_file_ctor_path_r.txt";
    std::string content = "Test content";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, content);
    
    Path filePath(testFile);
    File file(filePath, ReadOnly);
    EXPECT_TRUE(file.isValid());
    EXPECT_TRUE(file.isOpen());
    
    file.close();
    FileSystem::rmFile(testFile);
}

// 测试 File 构造函数 - Path对象 + WriteOnly
TEST(FileTest, Constructor_Path_WriteOnly) {
    std::string testFile = "./test_file_ctor_path_w.txt";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile);
    
    Path filePath(testFile);
    File file(filePath, WriteOnly);
    EXPECT_TRUE(file.isValid());
    EXPECT_TRUE(file.isOpen());
    
    file.close();
    FileSystem::rmFile(testFile);
}

// 测试 File 构造函数 - Path对象 + Unknown
TEST(FileTest, Constructor_Path_Unknown) {
    std::string testFile = "./test_file_ctor_path_u.txt";
    std::string content = "Test content";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, content);
    
    Path filePath(testFile);
    File file(filePath, Unknown);
    EXPECT_TRUE(file.isValid());
    EXPECT_FALSE(file.isOpen());
    
    FileSystem::rmFile(testFile);
}

// 测试 File 构造函数 - 无效文件路径
TEST(FileTest, Constructor_InvalidPath) {
    File file("/this/path/does/not/exist.txt", ReadOnly);
    EXPECT_FALSE(file.isValid());
    EXPECT_FALSE(file.isOpen());
}

// 测试 File 构造函数 - 目录路径（不是文件）
TEST(FileTest, Constructor_DirectoryPath) {
    File file(".", ReadOnly);
    EXPECT_FALSE(file.isValid());
}

// 测试移动构造函数 - 资源转移
TEST(FileTest, MoveConstructor_Transfer) {
    std::string testFile = "./test_file_move_ctor.txt";
    std::string content = "Move constructor test";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, content);
    
    File file1(testFile, ReadOnly);
    EXPECT_TRUE(file1.isOpen());
    
    File file2(std::move(file1));
    
    // 源对象被重置
    EXPECT_FALSE(file1.isOpen());
    EXPECT_FALSE(file1.isValid());
    
    // 目标对象获得资源
    EXPECT_TRUE(file2.isOpen());
    EXPECT_TRUE(file2.isValid());
    
    file2.close();
    FileSystem::rmFile(testFile);
}

// 测试移动构造函数 - 可以正常读取
TEST(FileTest, MoveConstructor_CanRead) {
    std::string testFile = "./test_file_move_ctor_read.txt";
    std::string content = "Hello, Move!";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, content);
    
    File file1(testFile, ReadOnly);
    File file2(std::move(file1));
    
    FileData data = file2.readAll();
    EXPECT_FALSE(data.empty());
    
    file2.close();
    FileSystem::rmFile(testFile);
}

// 测试移动赋值运算符 - 资源转移
TEST(FileTest, MoveAssignment_Transfer) {
    std::string testFile1 = "./test_file_move_asgn1.txt";
    std::string testFile2 = "./test_file_move_asgn2.txt";
    
    Path checkPath1(testFile1);
    if (checkPath1.isValid()) FileSystem::rmFile(testFile1);
    Path checkPath2(testFile2);
    if (checkPath2.isValid()) FileSystem::rmFile(testFile2);
    
    FileSystem::mkFile(testFile1, "content1");
    FileSystem::mkFile(testFile2, "content2");
    
    File file1(testFile1, ReadOnly);
    File file2(testFile2, ReadOnly);
    
    EXPECT_TRUE(file1.isOpen());
    EXPECT_TRUE(file2.isOpen());
    
    file2 = std::move(file1);
    
    // 源对象被重置
    EXPECT_FALSE(file1.isOpen());
    EXPECT_FALSE(file1.isValid());
    
    // 目标对象获得资源
    EXPECT_TRUE(file2.isOpen());
    EXPECT_TRUE(file2.isValid());
    
    file2.close();
    FileSystem::rmFile(testFile1);
    FileSystem::rmFile(testFile2);
}

// 测试移动赋值运算符 - 自赋值安全
TEST(FileTest, MoveAssignment_Self) {
    std::string testFile = "./test_file_move_self.txt";
    std::string content = "Self assignment test";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, content);
    
    File file(testFile, ReadOnly);
    EXPECT_TRUE(file.isOpen());
    
    File* ptr = &file;
    *ptr = std::move(file);
    
    // 自赋值后仍保持有效
    EXPECT_TRUE(file.isOpen());
    EXPECT_TRUE(file.isValid());
    
    file.close();
    FileSystem::rmFile(testFile);
}

// 测试 isValid() - 有效文件返回 true
TEST(FileTest, IsValid_True) {
    std::string testFile = "./test_file_valid_true.txt";
    std::string content = "Valid file";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, content);
    
    File file(testFile, ReadOnly);
    EXPECT_TRUE(file.isValid());
    
    file.close();
    FileSystem::rmFile(testFile);
}

// 测试 isValid() - 无效文件返回 false（目录）
TEST(FileTest, IsValid_False_Directory) {
    File file(".", ReadOnly);
    EXPECT_FALSE(file.isValid());
}

// 测试 isValid() - 无效文件返回 false（不存在）
TEST(FileTest, IsValid_False_NotExist) {
    File file("/nonexistent/path/file.txt", ReadOnly);
    EXPECT_FALSE(file.isValid());
}

// 测试 isOpen() - 打开的文件返回 true
TEST(FileTest, IsOpen_True) {
    std::string testFile = "./test_file_open_true.txt";
    std::string content = "Open test";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, content);
    
    File file(testFile, ReadOnly);
    EXPECT_TRUE(file.isOpen());
    
    file.close();
    FileSystem::rmFile(testFile);
}

// 测试 isOpen() - 未打开的文件返回 false
TEST(FileTest, IsOpen_False) {
    std::string testFile = "./test_file_open_false.txt";
    std::string content = "Not open test";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, content);
    
    File file(testFile, Unknown);
    EXPECT_FALSE(file.isOpen());
    
    FileSystem::rmFile(testFile);
}

// 测试 open() - 成功打开文件
TEST(FileTest, Open_Success) {
    std::string testFile = "./test_file_open_success.txt";
    std::string content = "Open success";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, content);
    
    File file(testFile, Unknown);
    EXPECT_FALSE(file.isOpen());
    
    EXPECT_TRUE(file.open(ReadOnly));
    EXPECT_TRUE(file.isOpen());
    
    file.close();
    FileSystem::rmFile(testFile);
}

// 测试 open() - 打开无效文件失败
TEST(FileTest, Open_Fail_Invalid) {
    File file("/nonexistent/path/file.txt", Unknown);
    EXPECT_FALSE(file.open(ReadOnly));
    EXPECT_FALSE(file.isOpen());
}

// 测试 open() - 打开目录失败
TEST(FileTest, Open_Fail_Directory) {
    File file(".", Unknown);
    EXPECT_FALSE(file.open(ReadOnly));
}

// 测试 close() - 正常关闭
TEST(FileTest, Close_Normal) {
    std::string testFile = "./test_file_close.txt";
    std::string content = "Close test";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, content);
    
    File file(testFile, ReadOnly);
    EXPECT_TRUE(file.isOpen());
    
    file.close();
    EXPECT_FALSE(file.isOpen());
    
    FileSystem::rmFile(testFile);
}

// 测试 close() - 多次关闭安全
TEST(FileTest, Close_Multiple) {
    std::string testFile = "./test_file_close_multi.txt";
    std::string content = "Multiple close test";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, content);
    
    File file(testFile, ReadOnly);
    file.close();
    file.close();
    file.close();
    
    EXPECT_FALSE(file.isOpen());
    
    FileSystem::rmFile(testFile);
}

// 测试 setPath() - 字符串参数，关闭已打开文件
TEST(FileTest, SetPath_String_ClosesFile) {
    std::string testFile1 = "./test_file_setpath_s1.txt";
    std::string testFile2 = "./test_file_setpath_s2.txt";
    
    Path checkPath1(testFile1);
    if (checkPath1.isValid()) FileSystem::rmFile(testFile1);
    Path checkPath2(testFile2);
    if (checkPath2.isValid()) FileSystem::rmFile(testFile2);
    
    FileSystem::mkFile(testFile1, "content1");
    FileSystem::mkFile(testFile2, "content2");
    
    File file(testFile1, ReadOnly);
    EXPECT_TRUE(file.isOpen());
    
    file.setPath(testFile2);
    // setPath 应该关闭原文件
    EXPECT_FALSE(file.isOpen());
    EXPECT_TRUE(file.isValid());
    
    FileSystem::rmFile(testFile1);
    FileSystem::rmFile(testFile2);
}

// 测试 setPath() - Path参数，关闭已打开文件
TEST(FileTest, SetPath_Path_ClosesFile) {
    std::string testFile1 = "./test_file_setpath_p1.txt";
    std::string testFile2 = "./test_file_setpath_p2.txt";
    
    Path checkPath1(testFile1);
    if (checkPath1.isValid()) FileSystem::rmFile(testFile1);
    Path checkPath2(testFile2);
    if (checkPath2.isValid()) FileSystem::rmFile(testFile2);
    
    FileSystem::mkFile(testFile1, "content1");
    FileSystem::mkFile(testFile2, "content2");
    
    File file(testFile1, ReadOnly);
    EXPECT_TRUE(file.isOpen());
    
    Path newPath(testFile2);
    file.setPath(newPath);
    // setPath 应该关闭原文件
    EXPECT_FALSE(file.isOpen());
    EXPECT_TRUE(file.isValid());
    
    FileSystem::rmFile(testFile1);
    FileSystem::rmFile(testFile2);
}

// 测试 setPath() - 设置到无效路径
TEST(FileTest, SetPath_Invalid) {
    std::string testFile = "./test_file_setpath_inv.txt";
    std::string content = "Valid content";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, content);
    
    File file(testFile, ReadOnly);
    EXPECT_TRUE(file.isValid());
    
    file.setPath("/nonexistent/path/file.txt");
    EXPECT_FALSE(file.isValid());
    
    file.close();
    FileSystem::rmFile(testFile);
}

// 测试 readAll() - 读取整个文件
TEST(FileTest, ReadAll_Success) {
    std::string testFile = "./test_file_readall.txt";
    std::string content = "Hello, World!";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, content);
    
    File file(testFile, ReadOnly);
    FileData data = file.readAll();
    
    EXPECT_FALSE(data.empty());
    EXPECT_GE(data.size(), content.size());
    
    file.close();
    FileSystem::rmFile(testFile);
}

// 测试 readAll() - 未打开文件返回空
TEST(FileTest, ReadAll_NotOpen) {
    std::string testFile = "./test_file_readall_no.txt";
    std::string content = "Content";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, content);
    
    File file(testFile, Unknown);
    FileData data = file.readAll();
    
    EXPECT_TRUE(data.empty());
    
    FileSystem::rmFile(testFile);
}

// 测试 readAll() - 无效文件返回空
TEST(FileTest, ReadAll_Invalid) {
    File file("/nonexistent/path/file.txt", ReadOnly);
    FileData data = file.readAll();
    EXPECT_TRUE(data.empty());
}

// 测试 read() - 读取指定长度
TEST(FileTest, Read_Length) {
    std::string testFile = "./test_file_read_len.txt";
    std::string content = "Hello, World!";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, content);
    
    File file(testFile, ReadOnly);
    FileData data = file.read(5);
    
    EXPECT_FALSE(data.empty());
    
    file.close();
    FileSystem::rmFile(testFile);
}

// 测试 read() - 读取长度超过文件大小
TEST(FileTest, Read_ExceedFileSize) {
    std::string testFile = "./test_file_read_exceed.txt";
    std::string content = "Hi";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, content);
    
    File file(testFile, ReadOnly);
    FileData data = file.read(1000);
    
    EXPECT_FALSE(data.empty());
    
    file.close();
    FileSystem::rmFile(testFile);
}

// 测试 read() - 读取0字节
TEST(FileTest, Read_Zero) {
    std::string testFile = "./test_file_read_zero.txt";
    std::string content = "Some content";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, content);
    
    File file(testFile, ReadOnly);
    FileData data = file.read(0);
    
    EXPECT_FALSE(data.empty());
    
    file.close();
    FileSystem::rmFile(testFile);
}

// 测试 write() - 写入完整数据
TEST(FileTest, Write_Full) {
    std::string testFile = "./test_file_write_full.txt";
    FileData data = {0x48, 0x65, 0x6C, 0x6C, 0x6F};
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile);
    
    File file(testFile, WriteOnly);
    EXPECT_TRUE(file.write(data));
    
    file.close();
    
    Path verifyPath(testFile);
    EXPECT_EQ(verifyPath.fileSize(), data.size());
    
    FileSystem::rmFile(testFile);
}

// 测试 write() - 写入指定长度
TEST(FileTest, Write_Length) {
    std::string testFile = "./test_file_write_len.txt";
    FileData data = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64};
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile);
    
    File file(testFile, WriteOnly);
    EXPECT_TRUE(file.write(data, 5));
    
    file.close();
    
    Path verifyPath(testFile);
    EXPECT_EQ(verifyPath.fileSize(), 5);
    
    FileSystem::rmFile(testFile);
}

// 测试 write() - 写入长度超过数据大小仍能成功
TEST(FileTest, Write_LengthExceedsData) {
    std::string testFile = "./test_file_write_exceed.txt";
    FileData data = {0x01, 0x02, 0x03};
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile);
    
    File file(testFile, WriteOnly);
    EXPECT_TRUE(file.write(data, 100));
    
    file.close();
    FileSystem::rmFile(testFile);
}

// 测试 write() - 写入0字节
TEST(FileTest, Write_ZeroLength) {
    std::string testFile = "./test_file_write_zero.txt";
    FileData data = {0x01, 0x02, 0x03};
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile);
    
    File file(testFile, WriteOnly);
    EXPECT_TRUE(file.write(data, 0));
    
    file.close();
    
    Path verifyPath(testFile);
    EXPECT_EQ(verifyPath.fileSize(), 0);
    
    FileSystem::rmFile(testFile);
}

// 测试 write() - 未打开文件失败
TEST(FileTest, Write_NotOpen) {
    std::string testFile = "./test_file_write_no.txt";
    FileData data = {0x01, 0x02};
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile);
    
    File file(testFile, Unknown);
    EXPECT_FALSE(file.write(data));
    
    FileSystem::rmFile(testFile);
}

// 测试 write() - 无效文件失败
TEST(FileTest, Write_Invalid) {
    FileData data = {0x01, 0x02};
    File file("/nonexistent/path/file.txt", WriteOnly);
    EXPECT_FALSE(file.write(data));
}

// 测试 write() - 空数据
TEST(FileTest, Write_EmptyData) {
    std::string testFile = "./test_file_write_empty.txt";
    FileData data;
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile);
    
    File file(testFile, WriteOnly);
    EXPECT_TRUE(file.write(data));
    
    file.close();
    FileSystem::rmFile(testFile);
}

// 测试多次写入 - 数据追加
TEST(FileTest, Write_Multiple) {
    std::string testFile = "./test_file_write_multi.txt";
    FileData data1 = {0x01, 0x02, 0x03};
    FileData data2 = {0x04, 0x05, 0x06};
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile);
    
    File file(testFile, WriteOnly);
    EXPECT_TRUE(file.write(data1));
    EXPECT_TRUE(file.write(data2));
    file.close();
    
    Path verifyPath(testFile);
    EXPECT_EQ(verifyPath.fileSize(), data1.size() + data2.size());
    
    FileSystem::rmFile(testFile);
}

// 测试读写完整流程 - 写入后读取验证内容
TEST(FileTest, WriteThenRead) {
    std::string testFile = "./test_file_wr.txt";
    FileData writeData = {0x48, 0x65, 0x6C, 0x6C, 0x6F};
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile);
    
    // 写入
    {
        File file(testFile, WriteOnly);
        EXPECT_TRUE(file.write(writeData));
        file.close();
    }
    
    // 读取并验证
    {
        File file(testFile, ReadOnly);
        FileData readData = file.readAll();
        
        EXPECT_GE(readData.size(), writeData.size());
        
        bool match = true;
        for (size_t i = 0; i < writeData.size(); ++i) {
            if (readData[i] != writeData[i]) {
                match = false;
                break;
            }
        }
        EXPECT_TRUE(match);
        
        file.close();
    }
    
    FileSystem::rmFile(testFile);
}

// 测试析构函数 - 自动关闭文件
TEST(FileTest, Destructor_AutoClose) {
    std::string testFile = "./test_file_dtor.txt";
    std::string content = "Destructor test";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, content);
    
    {
        File file(testFile, ReadOnly);
        EXPECT_TRUE(file.isOpen());
        // 离开作用域，析构函数自动关闭
    }
    
    // 文件应该可以被正常删除（已关闭）
    FileSystem::rmFile(testFile);
}

// 测试 OpenMode 组合 - Binary | ReadOnly
TEST(FileTest, OpenMode_BinaryRead) {
    std::string testFile = "./test_file_mode_br.txt";
    std::vector<uint8_t> data = {0x00, 0x01, 0x02, 0xFF};
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, data);
    
    File file(testFile, static_cast<OpenMode>(Binary | ReadOnly));
    EXPECT_TRUE(file.isOpen());
    
    file.close();
    FileSystem::rmFile(testFile);
}

// 测试 OpenMode 组合 - Text | ReadOnly
TEST(FileTest, OpenMode_TextRead) {
    std::string testFile = "./test_file_mode_tr.txt";
    std::string content = "Line 1\nLine 2";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, content);
    
    File file(testFile, static_cast<OpenMode>(Text | ReadOnly));
    EXPECT_TRUE(file.isOpen());
    
    file.close();
    FileSystem::rmFile(testFile);
}

// 测试 OpenMode 组合 - Binary | WriteOnly
TEST(FileTest, OpenMode_BinaryWrite) {
    std::string testFile = "./test_file_mode_bw.txt";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile);
    
    File file(testFile, static_cast<OpenMode>(Binary | WriteOnly));
    EXPECT_TRUE(file.isOpen());
    
    file.close();
    FileSystem::rmFile(testFile);
}

// 测试 OpenMode 组合 - Text | WriteOnly
TEST(FileTest, OpenMode_TextWrite) {
    std::string testFile = "./test_file_mode_tw.txt";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile);
    
    File file(testFile, static_cast<OpenMode>(Text | WriteOnly));
    EXPECT_TRUE(file.isOpen());
    
    file.close();
    FileSystem::rmFile(testFile);
}

// 测试 OpenMode 组合 - Binary | ReadWrite
TEST(FileTest, OpenMode_BinaryReadWrite) {
    std::string testFile = "./test_file_mode_brw.txt";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile);
    
    File file(testFile, static_cast<OpenMode>(Binary | ReadWrite));
    EXPECT_TRUE(file.isOpen());
    
    file.close();
    FileSystem::rmFile(testFile);
}

// 测试 OpenMode 组合 - Text | ReadWrite
TEST(FileTest, OpenMode_TextReadWrite) {
    std::string testFile = "./test_file_mode_trw.txt";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile);
    
    File file(testFile, static_cast<OpenMode>(Text | ReadWrite));
    EXPECT_TRUE(file.isOpen());
    
    file.close();
    FileSystem::rmFile(testFile);
}

// 测试 OpenMode 组合 - 复杂组合
TEST(FileTest, OpenMode_Complex) {
    std::string testFile = "./test_file_mode_complex.txt";
    std::string content = "Complex mode";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, content);
    
    File file(testFile, static_cast<OpenMode>(Binary | Text | ReadWrite));
    EXPECT_TRUE(file.isOpen());
    
    file.close();
    FileSystem::rmFile(testFile);
}

// ==================== 主函数 ====================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
