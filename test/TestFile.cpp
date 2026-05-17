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
#include "../src/OS/System.hpp"
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

// 测试 join() 方法
TEST(PathTest, Method_Join) {
    Path p(".");
    Path& result = p.join("subdir");
    EXPECT_EQ(&result, &p);
    EXPECT_FALSE(p.path().empty());
}

// 测试 parent() 方法
TEST(PathTest, Method_Parent) {
    Path p("./test_dir/test.txt");
    Path& result = p.parent();
    EXPECT_EQ(&result, &p);
    EXPECT_FALSE(p.path().empty());
}

// 测试 upper() 方法
TEST(PathTest, Method_Upper) {
    Path p("./test_dir");
    Path& result = p.upper();
    EXPECT_EQ(&result, &p);
    EXPECT_FALSE(p.path().empty());
}

// 测试静态方法 exist()
TEST(PathTest, Static_Exist) {
    EXPECT_TRUE(Path::exist("."));
    EXPECT_FALSE(Path::exist("/nonexistent/path/12345"));
}

// 测试静态方法 isDirectory()
TEST(PathTest, Static_IsDirectory) {
    EXPECT_TRUE(Path::isDirectory("."));
    
    std::string testFile = "./test_static_isfile.txt";
    FileSystem::mkFile(testFile);
    EXPECT_FALSE(Path::isDirectory(testFile));
    FileSystem::rmFile(testFile);
}

// 测试静态方法 isFile()
TEST(PathTest, Static_IsFile) {
    std::string testFile = "./test_static_isfile.txt";
    FileSystem::mkFile(testFile);
    EXPECT_TRUE(Path::isFile(testFile));
    FileSystem::rmFile(testFile);
    
    EXPECT_FALSE(Path::isFile("."));
}

// 测试静态方法 isSymbolLink()
TEST(PathTest, Static_IsSymbolLink) {
    EXPECT_FALSE(Path::isSymbolLink("."));
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

// 测试 readAllText() - 读取整个文件为文本
TEST(FileTest, ReadAllText_Success) {
    std::string testFile = "./test_file_readalltext.txt";
    std::string content = "Hello, World!";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, content);
    
    File file(testFile, ReadOnly);
    std::string text = file.readAllText();
    
    EXPECT_FALSE(text.empty());
    EXPECT_EQ(text, content);
    
    file.close();
    FileSystem::rmFile(testFile);
}

// 测试 readAllText() - 未打开文件返回空字符串
TEST(FileTest, ReadAllText_NotOpen) {
    std::string testFile = "./test_file_readalltext_no.txt";
    std::string content = "Content";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, content);
    
    File file(testFile, Unknown);
    std::string text = file.readAllText();
    
    EXPECT_TRUE(text.empty());
    
    FileSystem::rmFile(testFile);
}

// 测试 readText() - 读取指定长度的文本
TEST(FileTest, ReadText_Length) {
    std::string testFile = "./test_file_readtext_len.txt";
    std::string content = "Hello, World!";
    
    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);
    
    FileSystem::mkFile(testFile, content);
    
    File file(testFile, ReadOnly);
    std::string text = file.readText(5);
    
    EXPECT_FALSE(text.empty());
    EXPECT_EQ(text, "Hello");
    
    file.close();
    FileSystem::rmFile(testFile);
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
