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

TEST(PathTest, Assignment_Copy) {
    Path p1{"."};
    Path p2{".."};
    p2 = p1;
    EXPECT_TRUE(p2.isValid());
    EXPECT_EQ(p1.path(), p2.path());
}

TEST(PathTest, Assignment_Move) {
    Path p{"."};
    p = std::move(Path{".."});
    EXPECT_TRUE(p.isValid());
}

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

TEST(PathTest, Accessor_Path) {
    Path p(".");
    EXPECT_FALSE(p.path().empty());
}

TEST(PathTest, Accessor_ShortFileName) {
    Path p(".");
    EXPECT_FALSE(p.shortFileName().empty());
}

TEST(PathTest, Accessor_ExtensionName_File) {
    std::string testFile = "./test_extension.txt";
    if (!FileSystem::mkFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    Path p(testFile);
    EXPECT_EQ(p.extensionName(), "txt");

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(PathTest, Accessor_ExtensionName_Directory) {
    Path p(".");
    EXPECT_TRUE(p.extensionName().empty());
}

TEST(PathTest, Accessor_ExtensionName_NoExtension) {
    std::string testFile = "./test_no_extension";
    if (!FileSystem::mkFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    Path p(testFile);
    EXPECT_TRUE(p.extensionName().empty());

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(PathTest, Accessor_FileNameWithoutExtension_File) {
    std::string testFile = "./test_filename.txt";
    if (!FileSystem::mkFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    Path p(testFile);
    std::string name = p.fileNameWithoutExtension();
    EXPECT_FALSE(name.empty());

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(PathTest, Accessor_FileNameWithoutExtension_Directory) {
    Path p(".");
    EXPECT_EQ(p.fileNameWithoutExtension(), p.shortFileName());
}

TEST(PathTest, Accessor_ParentDirectory) {
    Path p("./test_dir/test.txt");
    std::string parent = p.parentDirectory();
    EXPECT_FALSE(parent.empty());
}

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

TEST(PathTest, TypeCheck_Valid) {
    Path p(".");
    EXPECT_TRUE(p.isValid());
}

TEST(PathTest, TypeCheck_Directory) {
    Path p(".");
    EXPECT_TRUE(p.isDirectory());
    EXPECT_FALSE(p.isFile());
}

TEST(PathTest, TypeCheck_File) {
    std::string testFile = "./test_isfile.txt";
    if (!FileSystem::mkFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    Path p(testFile);
    EXPECT_TRUE(p.isFile());
    EXPECT_FALSE(p.isDirectory());

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(PathTest, TypeCheck_IsSymbolLink) {
    Path p(".");
    EXPECT_FALSE(p.isSymbolLink());
}

TEST(PathTest, TypeCheck_FileType) {
    Path p(".");
    EXPECT_EQ(p.fileType(), FileType::Directory);
}

TEST(PathTest, Accessor_FileSize) {
    std::string testFile = "./test_size.txt";
    std::string content = "Hello, World!";
    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    Path p(testFile);
    EXPECT_EQ(p.fileSize(), content.size());

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(PathTest, Operator_Divide) {
    Path p(".");
    Path& result = p / "subdir";
    EXPECT_EQ(&result, &p);
    EXPECT_FALSE(p.path().empty());
}

TEST(PathTest, Method_Join) {
    Path p(".");
    Path& result = p.join("subdir");
    EXPECT_EQ(&result, &p);
    EXPECT_FALSE(p.path().empty());
}

TEST(PathTest, Method_Parent) {
    Path p("./test_dir/test.txt");
    Path& result = p.parent();
    EXPECT_EQ(&result, &p);
    EXPECT_FALSE(p.path().empty());
}

TEST(PathTest, Method_Upper) {
    Path p("./test_dir");
    Path& result = p.upper();
    EXPECT_EQ(&result, &p);
    EXPECT_FALSE(p.path().empty());
}

TEST(PathTest, Static_Exist) {
    EXPECT_TRUE(Path::exist("."));
    EXPECT_FALSE(Path::exist("/nonexistent/path/12345"));
}

TEST(PathTest, Static_IsDirectory) {
    EXPECT_TRUE(Path::isDirectory("."));

    std::string testFile = "./test_static_isfile.txt";
    if (!FileSystem::mkFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }
    EXPECT_FALSE(Path::isDirectory(testFile));
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(PathTest, Static_IsFile) {
    std::string testFile = "./test_static_isfile.txt";
    if (!FileSystem::mkFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }
    EXPECT_TRUE(Path::isFile(testFile));
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }

    EXPECT_FALSE(Path::isFile("."));
}

TEST(PathTest, Static_IsSymbolLink) {
    EXPECT_FALSE(Path::isSymbolLink("."));
}

TEST(PathTest, InvalidPath) {
    Path invalid("/this/path/does/not/exist/12345");
    EXPECT_FALSE(invalid.isValid());
    EXPECT_FALSE(invalid.isDirectory());
    EXPECT_FALSE(invalid.isFile());
    EXPECT_EQ(invalid.fileType(), FileType::Unknown);
}

TEST(PathTest, EmptyPath) {
    Path empty("");
    SUCCEED();
}

TEST(PathTest, Method_Unset) {
    Path p(".");
    EXPECT_TRUE(p.isValid());
    p.unset();
    EXPECT_FALSE(p.isValid());
    EXPECT_TRUE(p.path().empty());
}

TEST(PathTest, Accessor_LastAccessTime) {
    std::string testFile = "./test_access_time.txt";
    if (!FileSystem::mkFile(testFile, "test")) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    Path p(testFile);
    EXPECT_GT(p.lastAccessTime(), 0);

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(PathTest, Accessor_LastWriteTime) {
    std::string testFile = "./test_write_time.txt";
    if (!FileSystem::mkFile(testFile, "test")) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    Path p(testFile);
    EXPECT_GT(p.lastWriteTime(), 0);

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(PathTest, Accessor_LastCreateTime) {
    std::string testFile = "./test_create_time.txt";
    if (!FileSystem::mkFile(testFile, "test")) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    Path p(testFile);
    EXPECT_GT(p.lastCreateTime(), 0);

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(PathTest, Accessor_UserPermission) {
    std::string testFile = "./test_user_perm.txt";
    if (!FileSystem::mkFile(testFile, "test")) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    Path p(testFile);
    Permission perm = p.userPermission();
    // 在 Windows 上可能返回 None，在 Unix 上应该有值
    EXPECT_TRUE(perm == P_None || perm >= P_Read);

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(PathTest, Accessor_GroupPermission) {
    std::string testFile = "./test_group_perm.txt";
    if (!FileSystem::mkFile(testFile, "test")) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    Path p(testFile);
    Permission perm = p.groupPermission();
    EXPECT_TRUE(perm == P_None || perm >= P_Read);

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(PathTest, Accessor_OtherPermission) {
    std::string testFile = "./test_other_perm.txt";
    if (!FileSystem::mkFile(testFile, "test")) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    Path p(testFile);
    Permission perm = p.otherPermission();
    EXPECT_TRUE(perm == P_None || perm >= P_Read);

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}


TEST(DataUnitTest, ConvertDataSize_B_to_KiB) {
    size_t size = 2048;
    double result = convertDataSize(size, DataUnit::KiB, DataUnit::B);
    EXPECT_DOUBLE_EQ(result, 2.0);
}

TEST(DataUnitTest, ConvertDataSize_KiB_to_MiB) {
    size_t size = 1024;
    double result = convertDataSize(size, DataUnit::MiB, DataUnit::KiB);
    EXPECT_DOUBLE_EQ(result, 1.0);
}

TEST(DataUnitTest, ConvertDataSize_MiB_to_GiB) {
    size_t size = 1024;
    double result = convertDataSize(size, DataUnit::GiB, DataUnit::MiB);
    EXPECT_DOUBLE_EQ(result, 1.0);
}

TEST(DataUnitTest, ConvertDataSize_GiB_to_TiB) {
    size_t size = 1024;
    double result = convertDataSize(size, DataUnit::TiB, DataUnit::GiB);
    EXPECT_DOUBLE_EQ(result, 1.0);
}

TEST(DataUnitTest, ConvertDataSize_SameUnit) {
    size_t size = 100;
    double result = convertDataSize(size, DataUnit::MiB, DataUnit::MiB);
    EXPECT_DOUBLE_EQ(result, 100.0);
}

TEST(DataUnitTest, UserDefinedLiterals_B) {
    EXPECT_EQ(100_B, 100);
}

TEST(DataUnitTest, UserDefinedLiterals_KB) {
    EXPECT_EQ(1_KB, 1000);
}

TEST(DataUnitTest, UserDefinedLiterals_KiB) {
    EXPECT_EQ(1_KiB, 1024);
}

TEST(DataUnitTest, UserDefinedLiterals_MB) {
    EXPECT_EQ(1_MB, 1000000);
}

TEST(DataUnitTest, UserDefinedLiterals_MiB) {
    EXPECT_EQ(1_MiB, 1048576);
}

TEST(DataUnitTest, UserDefinedLiterals_GB) {
    EXPECT_EQ(1_GB, 1000000000);
}

TEST(DataUnitTest, UserDefinedLiterals_GiB) {
    EXPECT_EQ(1_GiB, 1073741824);
}

TEST(DataUnitTest, UserDefinedLiterals_TB) {
    EXPECT_EQ(1_TB, 1000000000000);
}

TEST(DataUnitTest, UserDefinedLiterals_TiB) {
    EXPECT_EQ(1_TiB, 1099511627776);
}


TEST(PermissionTest, PermissionValues) {
    EXPECT_EQ(static_cast<uint8_t>(P_None), 0);
    EXPECT_EQ(static_cast<uint8_t>(P_Execute), 1);
    EXPECT_EQ(static_cast<uint8_t>(P_Write), 2);
    EXPECT_EQ(static_cast<uint8_t>(P_WriteExec), 3);
    EXPECT_EQ(static_cast<uint8_t>(P_Read), 4);
    EXPECT_EQ(static_cast<uint8_t>(P_ReadExec), 5);
    EXPECT_EQ(static_cast<uint8_t>(P_ReadWrite), 6);
    EXPECT_EQ(static_cast<uint8_t>(P_All), 7);
}


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


// 移动后源对象会被重置为未打开状态

TEST(FileTest, TypeTrait_NoCopy) {
    EXPECT_FALSE(std::is_copy_constructible<File>::value);
    EXPECT_FALSE(std::is_copy_assignable<File>::value);
}

TEST(FileTest, TypeTrait_MoveOnly) {
    EXPECT_TRUE(std::is_move_constructible<File>::value);
    EXPECT_TRUE(std::is_move_assignable<File>::value);
}

TEST(FileTest, Constructor_String_ReadOnly) {
    std::string testFile = "./test_file_ctor_str_r.txt";
    std::string content = "Test content";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, ReadOnly);
    EXPECT_TRUE(file.isFile());
    EXPECT_TRUE(file.isOpen());

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, Constructor_String_WriteOnly) {
    std::string testFile = "./test_file_ctor_str_w.txt";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, WriteOnly);
    EXPECT_TRUE(file.isFile());
    EXPECT_TRUE(file.isOpen());

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, Constructor_String_ReadWrite) {
    std::string testFile = "./test_file_ctor_str_rw.txt";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, ReadWrite);
    EXPECT_TRUE(file.isFile());
    EXPECT_TRUE(file.isOpen());

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, Constructor_String_Unknown) {
    std::string testFile = "./test_file_ctor_str_u.txt";
    std::string content = "Test content";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, Unknown);
    EXPECT_TRUE(file.isFile());
    EXPECT_FALSE(file.isOpen());

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, Constructor_Path_ReadOnly) {
    std::string testFile = "./test_file_ctor_path_r.txt";
    std::string content = "Test content";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    Path filePath(testFile);
    File file(filePath, ReadOnly);
    EXPECT_TRUE(file.isFile());
    EXPECT_TRUE(file.isOpen());

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, Constructor_Path_WriteOnly) {
    std::string testFile = "./test_file_ctor_path_w.txt";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    Path filePath(testFile);
    File file(filePath, WriteOnly);
    EXPECT_TRUE(file.isFile());
    EXPECT_TRUE(file.isOpen());

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, Constructor_Path_Unknown) {
    std::string testFile = "./test_file_ctor_path_u.txt";
    std::string content = "Test content";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    Path filePath(testFile);
    File file(filePath, Unknown);
    EXPECT_TRUE(file.isFile());
    EXPECT_FALSE(file.isOpen());

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, Constructor_InvalidPath) {
    File file("/this/path/does/not/exist.txt", ReadOnly);
    EXPECT_FALSE(file.isFile());
    EXPECT_FALSE(file.isOpen());
}

TEST(FileTest, Constructor_DirectoryPath) {
    File file(".", ReadOnly);
    EXPECT_FALSE(file.isFile());
}

TEST(FileTest, MoveConstructor_Transfer) {
    std::string testFile = "./test_file_move_ctor.txt";
    std::string content = "Move constructor test";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file1(testFile, ReadOnly);
    EXPECT_TRUE(file1.isOpen());

    File file2(std::move(file1));

    // 源对象被重置
    EXPECT_FALSE(file1.isOpen());
    EXPECT_FALSE(file1.isFile());

    // 目标对象获得资源
    EXPECT_TRUE(file2.isOpen());
    EXPECT_TRUE(file2.isFile());

    file2.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, MoveConstructor_CanRead) {
    std::string testFile = "./test_file_move_ctor_read.txt";
    std::string content = "Hello, Move!";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file1(testFile, ReadOnly);
    File file2(std::move(file1));

    FileData data = file2.readAll();
    EXPECT_FALSE(data.empty());

    file2.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, MoveAssignment_Transfer) {
    std::string testFile1 = "./test_file_move_asgn1.txt";
    std::string testFile2 = "./test_file_move_asgn2.txt";

    Path checkPath1(testFile1);
    if (checkPath1.isValid()) FileSystem::rmFile(testFile1);
    Path checkPath2(testFile2);
    if (checkPath2.isValid()) FileSystem::rmFile(testFile2);

    if (!FileSystem::mkFile(testFile1, "content1")) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile(testFile1) failed: " << error;
    }
    if (!FileSystem::mkFile(testFile2, "content2")) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile(testFile2) failed: " << error;
    }

    File file1(testFile1, ReadOnly);
    File file2(testFile2, ReadOnly);

    EXPECT_TRUE(file1.isOpen());
    EXPECT_TRUE(file2.isOpen());

    file2 = std::move(file1);

    // 源对象被重置
    EXPECT_FALSE(file1.isOpen());
    EXPECT_FALSE(file1.isFile());

    // 目标对象获得资源
    EXPECT_TRUE(file2.isOpen());
    EXPECT_TRUE(file2.isFile());

    file2.close();
    if (!FileSystem::rmFile(testFile1)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile(testFile1) failed: " << error;
    }
    if (!FileSystem::rmFile(testFile2)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile(testFile2) failed: " << error;
    }
}

TEST(FileTest, MoveAssignment_Self) {
    std::string testFile = "./test_file_move_self.txt";
    std::string content = "Self assignment test";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, ReadOnly);
    EXPECT_TRUE(file.isOpen());

    File* ptr = &file;
    *ptr = std::move(file);

    // 自赋值后仍保持有效
    EXPECT_TRUE(file.isOpen());
    EXPECT_TRUE(file.isFile());

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, IsValid_True) {
    std::string testFile = "./test_file_valid_true.txt";
    std::string content = "Valid file";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, ReadOnly);
    EXPECT_TRUE(file.isFile());

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, IsValid_False_Directory) {
    File file(".", ReadOnly);
    EXPECT_FALSE(file.isFile());
}

TEST(FileTest, IsValid_False_NotExist) {
    File file("/nonexistent/path/file.txt", ReadOnly);
    EXPECT_FALSE(file.isFile());
}

TEST(FileTest, IsOpen_True) {
    std::string testFile = "./test_file_open_true.txt";
    std::string content = "Open test";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, ReadOnly);
    EXPECT_TRUE(file.isOpen());

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, IsOpen_False) {
    std::string testFile = "./test_file_open_false.txt";
    std::string content = "Not open test";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, Unknown);
    EXPECT_FALSE(file.isOpen());

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, Open_SUCCESS) {
    std::string testFile = "./test_file_open_success.txt";
    std::string content = "Open success";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, Unknown);
    EXPECT_FALSE(file.isOpen());

    if (!file.open(ReadOnly)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "open() failed: " << error;
    }
    EXPECT_TRUE(file.isOpen());

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, Open_Fail_Invalid) {
    File file("/nonexistent/path/file.txt", Unknown);
    EXPECT_FALSE(file.open(ReadOnly));
    EXPECT_FALSE(file.isOpen());
}

TEST(FileTest, Open_Fail_Directory) {
    File file(".", Unknown);
    EXPECT_FALSE(file.open(ReadOnly));
}

TEST(FileTest, Close_Normal) {
    std::string testFile = "./test_file_close.txt";
    std::string content = "Close test";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, ReadOnly);
    EXPECT_TRUE(file.isOpen());

    file.close();
    EXPECT_FALSE(file.isOpen());

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, Close_Multiple) {
    std::string testFile = "./test_file_close_multi.txt";
    std::string content = "Multiple close test";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, ReadOnly);
    file.close();
    file.close();
    file.close();

    EXPECT_FALSE(file.isOpen());

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, SetPath_String_ClosesFile) {
    std::string testFile1 = "./test_file_setpath_s1.txt";
    std::string testFile2 = "./test_file_setpath_s2.txt";

    Path checkPath1(testFile1);
    if (checkPath1.isValid()) FileSystem::rmFile(testFile1);
    Path checkPath2(testFile2);
    if (checkPath2.isValid()) FileSystem::rmFile(testFile2);

    if (!FileSystem::mkFile(testFile1, "content1")) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile(testFile1) failed: " << error;
    }
    if (!FileSystem::mkFile(testFile2, "content2")) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile(testFile2) failed: " << error;
    }

    File file(testFile1, ReadOnly);
    EXPECT_TRUE(file.isOpen());

    file.setPath(testFile2);
    // setPath 应该关闭原文件
    EXPECT_FALSE(file.isOpen());
    EXPECT_TRUE(file.isFile());

    if (!FileSystem::rmFile(testFile1)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile(testFile1) failed: " << error;
    }
    if (!FileSystem::rmFile(testFile2)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile(testFile2) failed: " << error;
    }
}

TEST(FileTest, SetPath_Path_ClosesFile) {
    std::string testFile1 = "./test_file_setpath_p1.txt";
    std::string testFile2 = "./test_file_setpath_p2.txt";

    Path checkPath1(testFile1);
    if (checkPath1.isValid()) FileSystem::rmFile(testFile1);
    Path checkPath2(testFile2);
    if (checkPath2.isValid()) FileSystem::rmFile(testFile2);

    if (!FileSystem::mkFile(testFile1, "content1")) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile(testFile1) failed: " << error;
    }
    if (!FileSystem::mkFile(testFile2, "content2")) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile(testFile2) failed: " << error;
    }

    File file(testFile1, ReadOnly);
    EXPECT_TRUE(file.isOpen());

    Path newPath(testFile2);
    file.setPath(newPath);
    // setPath 应该关闭原文件
    EXPECT_FALSE(file.isOpen());
    EXPECT_TRUE(file.isFile());

    if (!FileSystem::rmFile(testFile1)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile(testFile1) failed: " << error;
    }
    if (!FileSystem::rmFile(testFile2)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile(testFile2) failed: " << error;
    }
}

TEST(FileTest, SetPath_Invalid) {
    std::string testFile = "./test_file_setpath_inv.txt";
    std::string content = "Valid content";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, ReadOnly);
    EXPECT_TRUE(file.isFile());

    file.setPath("/nonexistent/path/file.txt");
    EXPECT_FALSE(file.isFile());

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, ReadAll_Success) {
    std::string testFile = "./test_file_readall.txt";
    std::string content = "Hello, World!";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, ReadOnly);
    FileData data = file.readAll();

    EXPECT_FALSE(data.empty());
    EXPECT_GE(data.size(), content.size());

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, ReadAll_NotOpen) {
    std::string testFile = "./test_file_readall_no.txt";
    std::string content = "Content";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, Unknown);
    FileData data = file.readAll();

    EXPECT_TRUE(data.empty());

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, ReadAll_Invalid) {
    File file("/nonexistent/path/file.txt", ReadOnly);
    FileData data = file.readAll();
    EXPECT_TRUE(data.empty());
}

TEST(FileTest, ReadAllText_Success) {
    std::string testFile = "./test_file_readalltext.txt";
    std::string content = "Hello, World!";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, ReadOnly);
    std::string text = file.readAllText();

    EXPECT_FALSE(text.empty());
    EXPECT_EQ(text, content);

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, ReadAllText_NotOpen) {
    std::string testFile = "./test_file_readalltext_no.txt";
    std::string content = "Content";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, Unknown);
    std::string text = file.readAllText();

    EXPECT_TRUE(text.empty());

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, ReadText_Length) {
    std::string testFile = "./test_file_readtext_len.txt";
    std::string content = "Hello, World!";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, ReadOnly);
    std::string text = file.readText(5);

    EXPECT_FALSE(text.empty());
    EXPECT_EQ(text, "Hello");

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, Read_Length) {
    std::string testFile = "./test_file_read_len.txt";
    std::string content = "Hello, World!";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, ReadOnly);
    FileData data = file.read(5);

    EXPECT_FALSE(data.empty());

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, Read_ExceedFileSize) {
    std::string testFile = "./test_file_read_exceed.txt";
    std::string content = "Hi";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, ReadOnly);
    FileData data = file.read(1000);

    EXPECT_FALSE(data.empty());

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, Read_Zero) {
    std::string testFile = "./test_file_read_zero.txt";
    std::string content = "Some content";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, ReadOnly);
    FileData data = file.read(0);

    EXPECT_FALSE(data.empty());

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, Write_Full) {
    std::string testFile = "./test_file_write_full.txt";
    FileData data = {0x48, 0x65, 0x6C, 0x6C, 0x6F};

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, WriteOnly);
    EXPECT_TRUE(file.write(data));

    file.close();

    Path verifyPath(testFile);
    EXPECT_EQ(verifyPath.fileSize(), data.size());

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, Write_Length) {
    std::string testFile = "./test_file_write_len.txt";
    FileData data = {0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64};

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, WriteOnly);
    EXPECT_TRUE(file.write(data, 5));

    file.close();

    Path verifyPath(testFile);
    EXPECT_EQ(verifyPath.fileSize(), 5);

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, Write_LengthExceedsData) {
    std::string testFile = "./test_file_write_exceed.txt";
    FileData data = {0x01, 0x02, 0x03};

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, WriteOnly);
    EXPECT_TRUE(file.write(data, 100));

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, Write_ZeroLength) {
    std::string testFile = "./test_file_write_zero.txt";
    FileData data = {0x01, 0x02, 0x03};

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, WriteOnly);
    EXPECT_TRUE(file.write(data, 0));

    file.close();

    Path verifyPath(testFile);
    EXPECT_EQ(verifyPath.fileSize(), 0);

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, Write_NotOpen) {
    std::string testFile = "./test_file_write_no.txt";
    FileData data = {0x01, 0x02};

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, Unknown);
    EXPECT_FALSE(file.write(data));

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, Write_Invalid) {
    FileData data = {0x01, 0x02};
    File file("/nonexistent/path/file.txt", WriteOnly);
    EXPECT_FALSE(file.write(data));
}

TEST(FileTest, Write_EmptyData) {
    std::string testFile = "./test_file_write_empty.txt";
    FileData data;

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, WriteOnly);
    EXPECT_TRUE(file.write(data));

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, Write_Multiple) {
    std::string testFile = "./test_file_write_multi.txt";
    FileData data1 = {0x01, 0x02, 0x03};
    FileData data2 = {0x04, 0x05, 0x06};

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, WriteOnly);
    EXPECT_TRUE(file.write(data1));
    EXPECT_TRUE(file.write(data2));
    file.close();

    Path verifyPath(testFile);
    EXPECT_EQ(verifyPath.fileSize(), data1.size() + data2.size());

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, WriteThenRead) {
    std::string testFile = "./test_file_wr.txt";
    FileData writeData = {0x48, 0x65, 0x6C, 0x6C, 0x6F};

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

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

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, Destructor_AutoClose) {
    std::string testFile = "./test_file_dtor.txt";
    std::string content = "Destructor test";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    {
        File file(testFile, ReadOnly);
        EXPECT_TRUE(file.isOpen());
        // 离开作用域，析构函数自动关闭
    }

    // 文件应该可以被正常删除（已关闭）
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, OpenMode_AppendWrite) {
    std::string testFile = "./test_file_mode_aw.txt";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, static_cast<OpenMode>(Append | WriteOnly));
    EXPECT_TRUE(file.isOpen());

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, OpenMode_AppendReadWrite) {
    std::string testFile = "./test_file_mode_arw.txt";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, static_cast<OpenMode>(Append | ReadWrite));
    EXPECT_TRUE(file.isOpen());

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, WriteLine_Success) {
    std::string testFile = "./test_file_writeline.txt";
    std::string line1 = "Hello, World!";
    std::string line2 = "Second line";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, WriteOnly);
    EXPECT_TRUE(file.writeLine(line1));
    EXPECT_TRUE(file.writeLine(line2));
    file.close();

    // 读取并验证
    File readFile(testFile, ReadOnly);
    std::string content = readFile.readAllText();
    readFile.close();

    EXPECT_NE(content.find(line1), std::string::npos);
    EXPECT_NE(content.find(line2), std::string::npos);

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, WriteLine_NotOpen) {
    std::string testFile = "./test_file_writeline_no.txt";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, Unknown);
    EXPECT_FALSE(file.writeLine("test"));

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, WriteLine_Invalid) {
    File file("/nonexistent/path/file.txt", WriteOnly);
    EXPECT_FALSE(file.writeLine("test"));
}

TEST(FileTest, IsEOF_AtEnd) {
    std::string testFile = "./test_file_eof.txt";
    std::string content = "EOF test";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, ReadOnly);
    file.readAll();
    EXPECT_TRUE(file.isEOF());

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, IsEOF_NotOpen) {
    std::string testFile = "./test_file_eof_no.txt";
    std::string content = "Content";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, Unknown);
    EXPECT_TRUE(file.isEOF());

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, FileSize_Success) {
    std::string testFile = "./test_file_size.txt";
    std::string content = "File size test content";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, ReadOnly);
    EXPECT_EQ(file.fileSize(), content.size());

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, FileSize_NotOpen) {
    std::string testFile = "./test_file_size_no.txt";
    std::string content = "Content";

    Path checkPath(testFile);
    if (checkPath.isValid()) FileSystem::rmFile(testFile);

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, Unknown);
    EXPECT_EQ(file.fileSize(), 0);

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, FileSize_Invalid) {
    File file("/nonexistent/path/file.txt", ReadOnly);
    EXPECT_EQ(file.fileSize(), 0);
}

TEST(FileTest, IsNull_False) {
    std::string testFile = "./test_file_notnull.txt";
    if (!FileSystem::mkFile(testFile, "test")) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, ReadOnly);
    EXPECT_FALSE(file.isNull());

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, IsNull_True) {
    File file("/nonexistent/path/file.txt", ReadOnly);
    EXPECT_TRUE(file.isNull());
}

TEST(FileTest, ReadLine_Success) {
    std::string testFile = "./test_file_readline.txt";
    std::string content = "Line 1\nLine 2\nLine 3";

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, ReadOnly);
    std::string line1 = file.readLine();
    EXPECT_EQ(line1, "Line 1");

    std::string line2 = file.readLine();
    EXPECT_EQ(line2, "Line 2");

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, ReadLine_NotOpen) {
    std::string testFile = "./test_file_readline_no.txt";
    if (!FileSystem::mkFile(testFile, "test")) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, Unknown);
    std::string line = file.readLine();
    EXPECT_TRUE(line.empty());

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, Write_CharPtr) {
    std::string testFile = "./test_file_write_char.txt";
    const char* data = "Hello";

    if (!FileSystem::mkFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, WriteOnly);
    EXPECT_TRUE(file.write(data, 5));

    file.close();

    Path verifyPath(testFile);
    EXPECT_EQ(verifyPath.fileSize(), 5);

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, Write_String) {
    std::string testFile = "./test_file_write_str.txt";
    std::string data = "Hello, String!";

    if (!FileSystem::mkFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, WriteOnly);
    EXPECT_TRUE(file.write(data));

    file.close();

    Path verifyPath(testFile);
    EXPECT_EQ(verifyPath.fileSize(), data.size());

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, MoveToStart) {
    std::string testFile = "./test_file_movestart.txt";
    std::string content = "Test content";

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, ReadOnly);
    file.read(5);
    file.moveToStart();

    std::string text = file.readText(4);
    EXPECT_EQ(text, "Test");

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, MoveToEnd) {
    std::string testFile = "./test_file_moveend.txt";
    std::string content = "Test content";

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, ReadOnly);
    file.moveToEnd();
    EXPECT_TRUE(file.isEOF());

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, MoveTo_Positive) {
    std::string testFile = "./test_file_moveto_pos.txt";
    std::string content = "Hello, World!";

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, ReadOnly);
    file.moveTo(7);

    std::string text = file.readText(5);
    EXPECT_EQ(text, "World");

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, MoveTo_Negative) {
    std::string testFile = "./test_file_moveto_neg.txt";
    std::string content = "Hello, World!";

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, ReadOnly);
    file.moveTo(-6);

    std::string text = file.readText(5);
    EXPECT_EQ(text, "World");

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, Accessor_Path) {
    std::string testFile = "./test_file_path.txt";
    if (!FileSystem::mkFile(testFile, "test")) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, ReadOnly);
    std::string path = file.path();
    EXPECT_FALSE(path.empty());

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}

TEST(FileTest, Accessor_FileName) {
    std::string testFile = "./test_file_name.txt";
    if (!FileSystem::mkFile(testFile, "test")) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }

    File file(testFile, ReadOnly);
    std::string name = file.fileName();
    EXPECT_FALSE(name.empty());
    EXPECT_NE(name.find("test_file_name"), std::string::npos);

    file.close();
    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

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
