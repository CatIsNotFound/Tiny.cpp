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
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

using namespace Tiny::OS;

// ============================================================================
// Test helpers
// ============================================================================

static const char* kTestFileName = "tiny_test_file.tmp";
static const char* kTestPathName = "tiny_test_path.txt";
static const char* kTestContent = "Hello Tiny!\nThis is line 2.\nline3";

static size_t contentLen() {
    return sizeof("Hello Tiny!\nThis is line 2.\nline3") - 1;
}

static void removeTestFile(const char* name) {
    std::remove(name);
}

/// Record the last system error info (project convention for system-related operations)
static void recordLastError(const char* context) {
    std::string info;
    int code = 0;
    lastSystemError(info, &code);
    SUCCEED() << context << " | last system error code: " << code
              << ", info: " << (info.empty() ? "<none>" : info);
}

// ============================================================================
// Data size literal Tests
// ============================================================================

TEST(DataSizeLiteralTest, ByteLiterals) {
    EXPECT_EQ(1_B, 1u);
    EXPECT_EQ(0_B, 0u);
    EXPECT_EQ(100_B, 100u);
}

TEST(DataSizeLiteralTest, DecimalLiterals) {
    EXPECT_EQ(1_KB, 1000u);
    EXPECT_EQ(2_KB, 2000u);
    EXPECT_EQ(1_MB, 1000000u);
    EXPECT_EQ(1_GB, 1000000000u);
    EXPECT_EQ(1_TB, 1000000000000u);
}

TEST(DataSizeLiteralTest, BinaryLiterals) {
    EXPECT_EQ(1_KiB, 1024u);
    EXPECT_EQ(2_KiB, 2048u);
    EXPECT_EQ(1_MiB, 1048576u);
    EXPECT_EQ(1_GiB, 1073741824u);
    EXPECT_EQ(1_TiB, 1099511627776u);
}

// ============================================================================
// DataUnit Tests
// ============================================================================

TEST(DataUnitTest, ConvertSameUnit) {
    EXPECT_DOUBLE_EQ(convertDataSize(1024, DataUnit::B, DataUnit::B), 1024.0);
    EXPECT_DOUBLE_EQ(convertDataSize(1024, DataUnit::KiB, DataUnit::KiB), 1024.0);
}

TEST(DataUnitTest, ConvertUpward) {
    EXPECT_DOUBLE_EQ(convertDataSize(1024, DataUnit::KiB, DataUnit::B), 1.0);
    EXPECT_DOUBLE_EQ(convertDataSize(1048576, DataUnit::MiB, DataUnit::B), 1.0);
    EXPECT_DOUBLE_EQ(convertDataSize(2048, DataUnit::MiB, DataUnit::KiB), 2.0);
    EXPECT_DOUBLE_EQ(convertDataSize(1073741824, DataUnit::GiB, DataUnit::B), 1.0);
    EXPECT_DOUBLE_EQ(convertDataSize(1099511627776, DataUnit::TiB, DataUnit::B), 1.0);
}

TEST(DataUnitTest, ConvertDownward) {
    EXPECT_DOUBLE_EQ(convertDataSize(1, DataUnit::B, DataUnit::KiB), 1024.0);
    EXPECT_DOUBLE_EQ(convertDataSize(1, DataUnit::B, DataUnit::MiB), 1048576.0);
    EXPECT_DOUBLE_EQ(convertDataSize(2, DataUnit::KiB, DataUnit::MiB), 2048.0);
}

TEST(DataUnitTest, ConvertRoundTrip) {
    auto up = convertDataSize(4096, DataUnit::KiB, DataUnit::B);
    auto back = convertDataSize(static_cast<size_t>(up), DataUnit::B, DataUnit::KiB);
    EXPECT_DOUBLE_EQ(back, 4096.0);
}

TEST(DataUnitTest, AutoConvertZero) {
    DataUnit unit = DataUnit::TiB;
    auto value = autoConvertDataSize(0, unit);
    EXPECT_DOUBLE_EQ(value, 0.0);
    EXPECT_EQ(unit, DataUnit::B);
}

TEST(DataUnitTest, AutoConvertBytes) {
    DataUnit unit = DataUnit::TiB;
    auto value = autoConvertDataSize(512, unit);
    EXPECT_DOUBLE_EQ(value, 512.0);
    EXPECT_EQ(unit, DataUnit::B);
}

TEST(DataUnitTest, AutoConvertKib) {
    DataUnit unit = DataUnit::B;
    auto value = autoConvertDataSize(2048, unit);
    EXPECT_DOUBLE_EQ(value, 2.0);
    EXPECT_EQ(unit, DataUnit::KiB);
}

TEST(DataUnitTest, AutoConvertMib) {
    DataUnit unit = DataUnit::B;
    auto value = autoConvertDataSize(2u * 1024u * 1024u, unit);
    EXPECT_DOUBLE_EQ(value, 2.0);
    EXPECT_EQ(unit, DataUnit::MiB);
}

TEST(DataUnitTest, AutoConvertOverflowClampToTib) {
    DataUnit unit = DataUnit::B;
    autoConvertDataSize(1024ull * 1024ull * 1024ull * 1024ull * 1024ull, unit);
    EXPECT_EQ(unit, DataUnit::TiB);
}

TEST(DataUnitTest, UnitNames) {
    EXPECT_STREQ(dataUnitName(DataUnit::B), "B");
    EXPECT_STREQ(dataUnitName(DataUnit::KiB), "KiB");
    EXPECT_STREQ(dataUnitName(DataUnit::MiB), "MiB");
    EXPECT_STREQ(dataUnitName(DataUnit::GiB), "GiB");
    EXPECT_STREQ(dataUnitName(DataUnit::TiB), "TiB");
}

// ============================================================================
// FileType Tests
// ============================================================================

TEST(FileTypeTest, TypeNames) {
    EXPECT_STREQ(fileTypeName(FileType::Unknown), "Unknown");
    EXPECT_STREQ(fileTypeName(FileType::Directory), "Directory");
    EXPECT_STREQ(fileTypeName(FileType::File), "File");
    EXPECT_STREQ(fileTypeName(FileType::SymbolLink), "Symbol Link");
    EXPECT_STREQ(fileTypeName(FileType::Device), "Device");
    EXPECT_STREQ(fileTypeName(FileType::Socket), "Socket");
}

// ============================================================================
// Permission Tests
// ============================================================================

TEST(PermissionTest, EnumValues) {
    EXPECT_EQ(static_cast<int>(P_None), 0);
    EXPECT_EQ(static_cast<int>(P_Execute), 1);
    EXPECT_EQ(static_cast<int>(P_Write), 2);
    EXPECT_EQ(static_cast<int>(P_WriteExec), 3);
    EXPECT_EQ(static_cast<int>(P_Read), 4);
    EXPECT_EQ(static_cast<int>(P_ReadExec), 5);
    EXPECT_EQ(static_cast<int>(P_ReadWrite), 6);
    EXPECT_EQ(static_cast<int>(P_All), 7);
}

TEST(PermissionTest, BitwiseCombination) {
    Permission perm = P_None;
    perm = static_cast<Permission>(perm | P_Read);
    EXPECT_EQ(perm, P_Read);
    perm = static_cast<Permission>(perm | P_Write);
    EXPECT_EQ(perm, P_ReadWrite);
    perm = static_cast<Permission>(perm | P_Execute);
    EXPECT_EQ(perm, P_All);
}

// ============================================================================
// Path Tests (string & filesystem)
// ============================================================================

class PathTest : public ::testing::Test {
protected:
    void SetUp() override {
        removeTestFile(kTestPathName);
        File file(kTestPathName, WriteOnly);
        ASSERT_TRUE(file.isOpen());
        ASSERT_TRUE(file.write("path test content"));
        file.close();
        recordLastError("PathTest::SetUp");
    }
    void TearDown() override {
        removeTestFile(kTestPathName);
    }
};

TEST_F(PathTest, ExistingFile) {
    Path path(kTestPathName);
    EXPECT_TRUE(path.isValid());
    EXPECT_TRUE(path.isFile());
    EXPECT_FALSE(path.isDirectory());
    EXPECT_EQ(path.fileType(), FileType::File);
    EXPECT_GT(path.fileSize(), 0u);
    EXPECT_EQ(path.fileSize(), strlen("path test content"));
}

TEST_F(PathTest, ShortNameAndExtension) {
    Path path(kTestPathName);
    EXPECT_EQ(path.shortFileName(), std::string("tiny_test_path.txt"));
    EXPECT_EQ(path.extensionName(), std::string("txt"));
    EXPECT_EQ(path.fileNameWithoutExtension(), std::string("tiny_test_path"));
}

TEST_F(PathTest, DirectoryType) {
    Path path(".");
    EXPECT_TRUE(path.isValid());
    EXPECT_TRUE(path.isDirectory());
    EXPECT_FALSE(path.isFile());
    EXPECT_EQ(path.fileType(), FileType::Directory);
}

TEST_F(PathTest, DirectoryExtensionIsEmpty) {
    Path path(".");
    EXPECT_TRUE(path.extensionName().empty());
    EXPECT_EQ(path.fileNameWithoutExtension(), path.shortFileName());
}

TEST_F(PathTest, ParentDirectoryString) {
    Path path(kTestPathName);
    auto parent = path.parentDirectory();
    EXPECT_FALSE(parent.empty());
    EXPECT_EQ(parent.find("tiny_test_path.txt"), std::string::npos);
}

TEST_F(PathTest, NonExistentPath) {
    Path path("no_such_file_or_dir_tiny_test_xyz");
    EXPECT_FALSE(path.isValid());
    EXPECT_FALSE(path.isFile());
    EXPECT_FALSE(path.isDirectory());
    EXPECT_EQ(path.fileType(), FileType::Unknown);
    EXPECT_FALSE(Path::exist("no_such_file_or_dir_tiny_test_xyz"));
    EXPECT_FALSE(Path::isFile("no_such_file_or_dir_tiny_test_xyz"));
    EXPECT_FALSE(Path::isDirectory("no_such_file_or_dir_tiny_test_xyz"));
    recordLastError("PathTest::NonExistentPath");
}

TEST_F(PathTest, EmptyPath) {
    Path path("");
    EXPECT_FALSE(path.isValid());
    EXPECT_TRUE(path.path().empty());
}

TEST_F(PathTest, StaticExistHelpers) {
    EXPECT_TRUE(Path::exist(kTestPathName));
    EXPECT_TRUE(Path::isFile(kTestPathName));
    EXPECT_FALSE(Path::isDirectory(kTestPathName));
    EXPECT_TRUE(Path::exist("."));
    EXPECT_TRUE(Path::isDirectory("."));
    EXPECT_FALSE(Path::isFile("."));
}

TEST_F(PathTest, CopyAndMove) {
    Path src(kTestPathName);
    Path copied(src);
    EXPECT_TRUE(copied.isValid());
    EXPECT_EQ(copied.path(), src.path());
    EXPECT_EQ(copied.fileSize(), src.fileSize());

    Path moved(std::move(copied));
    EXPECT_TRUE(moved.isValid());
    EXPECT_EQ(moved.path(), src.path());

    Path assigned("");
    assigned = src;
    EXPECT_TRUE(assigned.isValid());
    EXPECT_EQ(assigned.path(), src.path());

    Path move_assigned("");
    move_assigned = Path(kTestPathName);
    EXPECT_TRUE(move_assigned.isValid());
    EXPECT_EQ(move_assigned.path(), src.path());
}

TEST_F(PathTest, SetPathAndReset) {
    Path path("no_such_file_or_dir_tiny_test_xyz");
    EXPECT_FALSE(path.isValid());

    path.setPath(kTestPathName);
    EXPECT_TRUE(path.isValid());
    // checkPath normalizes the stored path to an absolute path
    EXPECT_NE(path.path().find(kTestPathName), std::string::npos);

    EXPECT_TRUE(path.reset());
    EXPECT_TRUE(path.isValid());

    path.setPath(std::string("no_such_file_or_dir_tiny_test_xyz"));
    EXPECT_FALSE(path.reset());
    EXPECT_FALSE(path.isValid());
    recordLastError("PathTest::SetPathAndReset");
}

TEST_F(PathTest, Unset) {
    Path path(kTestPathName);
    ASSERT_TRUE(path.isValid());
    path.unset();
    EXPECT_TRUE(path.path().empty());
    EXPECT_FALSE(path.isValid());
    EXPECT_EQ(path.fileType(), FileType::Unknown);
    EXPECT_EQ(path.fileSize(), 0u);
}

TEST_F(PathTest, JoinAndOperator) {
    Path path(".");
    path.join(kTestPathName);
    EXPECT_NE(path.path().find(kTestPathName), std::string::npos);
    bool has_sep = path.path().find("\\") != std::string::npos ||
                   path.path().find("/") != std::string::npos;
    EXPECT_TRUE(has_sep);
    EXPECT_TRUE(path.isFile());

    Path path2(".");
    path2 / kTestPathName;
    EXPECT_NE(path2.path().find(kTestPathName), std::string::npos);
    EXPECT_TRUE(path2.isFile());
}

TEST_F(PathTest, UniqueID) {
    Path path(kTestPathName);
    EXPECT_NE(path.uniqueID(), 0u);
}

TEST_F(PathTest, Timestamps) {
    Path path(kTestPathName);
    ASSERT_TRUE(path.isValid());
    EXPECT_GT(path.lastAccessTime(), 0);
    EXPECT_GT(path.lastWriteTime(), 0);
    EXPECT_GT(path.lastCreateTime(), 0);
}

TEST_F(PathTest, OwnerAndGroup) {
    Path path(kTestPathName);
    ASSERT_TRUE(path.isValid());
    bool ok = false;
    auto owner = path.ownerName(&ok);
    EXPECT_TRUE(ok);
    EXPECT_FALSE(owner.empty());
    ok = false;
    auto group = path.groupName(&ok);
    EXPECT_TRUE(ok);
    EXPECT_FALSE(group.empty());
}

TEST_F(PathTest, PermissionsInValidRange) {
    Path path(kTestPathName);
    ASSERT_TRUE(path.isValid());
    EXPECT_LE(static_cast<int>(path.userPermission()), static_cast<int>(P_All));
    EXPECT_LE(static_cast<int>(path.groupPermission()), static_cast<int>(P_All));
    EXPECT_LE(static_cast<int>(path.otherPermission()), static_cast<int>(P_All));
}

// ============================================================================
// File open/close Tests
// ============================================================================

TEST(FileOpenCloseTest, OpenNonExistentReadOnly) {
    removeTestFile(kTestFileName);
    File file(kTestFileName, Unknown);
    EXPECT_FALSE(file.isOpen());
    EXPECT_FALSE(file.open(ReadOnly));
    EXPECT_FALSE(file.isOpen());
    recordLastError("FileOpenCloseTest::OpenNonExistentReadOnly");
}

TEST(FileOpenCloseTest, OpenUnknownMode) {
    removeTestFile(kTestFileName);
    File file(kTestFileName, Unknown);
    EXPECT_FALSE(file.open(Unknown));
    EXPECT_FALSE(file.isOpen());
    recordLastError("FileOpenCloseTest::OpenUnknownMode");
}

TEST(FileOpenCloseTest, OpenDirectory) {
    File file(".", ReadOnly);
    EXPECT_FALSE(file.open(ReadOnly));
    EXPECT_FALSE(file.isOpen());
    recordLastError("FileOpenCloseTest::OpenDirectory");
}

TEST(FileOpenCloseTest, NullAndIsFile) {
    File valid(kTestFileName, WriteOnly);
    EXPECT_TRUE(valid.isOpen());
    EXPECT_TRUE(valid.isFile());
    EXPECT_FALSE(valid.isNull());
    EXPECT_FALSE(valid.path().empty());
    valid.close();

    File invalid("no_such_file_or_dir_tiny_test_xyz", ReadOnly);
    EXPECT_FALSE(invalid.isFile());
    EXPECT_TRUE(invalid.isNull());
    recordLastError("FileOpenCloseTest::NullAndIsFile");
}

TEST(FileOpenCloseTest, OpenWriteOnlyCreatesFile) {
    removeTestFile(kTestFileName);
    {
        File file(kTestFileName, WriteOnly);
        ASSERT_TRUE(file.isOpen());
        EXPECT_TRUE(file.isOpen());
        // Path is re-validated after creating the file
        EXPECT_TRUE(file.isFile());
        file.close();
        EXPECT_FALSE(file.isOpen());
    }
    EXPECT_TRUE(Path::exist(kTestFileName));
    removeTestFile(kTestFileName);
}

// ============================================================================
// File read/write Tests
// ============================================================================

class FileIOTest : public ::testing::Test {
protected:
    void SetUp() override {
        removeTestFile(kTestFileName);
        File file(kTestFileName, WriteOnly);
        ASSERT_TRUE(file.isOpen());
        ASSERT_TRUE(file.write(std::string(kTestContent)));
        file.close();
        recordLastError("FileIOTest::SetUp");
    }
    void TearDown() override {
        removeTestFile(kTestFileName);
    }
};

TEST_F(FileIOTest, ReadText) {
    File file(kTestFileName, ReadOnly);
    ASSERT_TRUE(file.isOpen());
    EXPECT_EQ(file.fileSize(), contentLen());

    auto first = file.readText(11);
    EXPECT_EQ(first.substr(0, 11), std::string("Hello Tiny!"));
    auto second = file.readText(1);
    EXPECT_EQ(second.substr(0, 1), std::string("\n"));
}

TEST_F(FileIOTest, ReadWithLength) {
    File file(kTestFileName, ReadOnly);
    ASSERT_TRUE(file.isOpen());

    auto data = file.read(5);
    ASSERT_GE(data.size(), 5u);
    std::string head(data.begin(), data.begin() + 5);
    EXPECT_EQ(head, std::string("Hello"));

    auto data2 = file.read(6);
    std::string tail(data2.begin(), data2.begin() + 6);
    EXPECT_EQ(tail, std::string(" Tiny!"));
}

TEST_F(FileIOTest, ReadAllAndReadAllText) {
    File file(kTestFileName, ReadOnly);
    ASSERT_TRUE(file.isOpen());

    std::string all = file.readAllText();
    EXPECT_EQ(all, std::string(kTestContent));

    file.moveToStart();
    auto raw = file.readAll();
    ASSERT_GE(raw.size(), contentLen());
    std::string content(raw.begin(), raw.begin() + static_cast<long>(contentLen()));
    EXPECT_EQ(content, std::string(kTestContent));
}

TEST_F(FileIOTest, ReadLine) {
    File file(kTestFileName, ReadOnly);
    ASSERT_TRUE(file.isOpen());

    EXPECT_EQ(file.readLine(), std::string("Hello Tiny!"));
    EXPECT_EQ(file.readLine(), std::string("This is line 2."));
    EXPECT_EQ(file.readLine(), std::string("line3"));
}

TEST_F(FileIOTest, ReadLineWithLimit) {
    File file(kTestFileName, ReadOnly);
    ASSERT_TRUE(file.isOpen());

    auto part = file.readLine(5);
    EXPECT_EQ(part, std::string("Hello"));
    EXPECT_FALSE(file.isEOF());
}

TEST_F(FileIOTest, IsEOFAndPosition) {
    File file(kTestFileName, ReadOnly);
    ASSERT_TRUE(file.isOpen());
    EXPECT_FALSE(file.isEOF());

    file.moveToEnd();
    EXPECT_TRUE(file.isEOF());

    file.moveToStart();
    EXPECT_FALSE(file.isEOF());
    (void)file.readAllText();
    EXPECT_TRUE(file.isEOF());
}

TEST_F(FileIOTest, MoveTo) {
    File file(kTestFileName, ReadOnly);
    ASSERT_TRUE(file.isOpen());

    file.moveTo(6, File::Where::Begin);
    auto text = file.readText(4);
    EXPECT_EQ(text.substr(0, 4), std::string("Tiny"));

    file.moveTo(-4, File::Where::Current);
    text = file.readText(4);
    EXPECT_EQ(text.substr(0, 4), std::string("Tiny"));

    file.moveTo(100000, File::Where::Begin);
    EXPECT_TRUE(file.isEOF());

    file.moveTo(3, File::Where::Begin);
    file.moveTo(2, File::Where::Current);
    text = file.readText(2);
    EXPECT_EQ(text.substr(0, 2), std::string(" T"));
}

TEST_F(FileIOTest, BinaryWriteAndRead) {
    removeTestFile(kTestFileName);
    File file(kTestFileName, WriteOnly);
    ASSERT_TRUE(file.isOpen());

    FileData out;
    out.push_back(0x00);
    out.push_back(0x01);
    out.push_back(0xFF);
    out.push_back(0x7F);
    out.push_back(0x80);
    ASSERT_TRUE(file.write(out));
    ASSERT_TRUE(file.write(out, out.size()));
    file.close();

    File in(kTestFileName, ReadOnly);
    ASSERT_TRUE(in.isOpen());
    auto raw = in.readAll();
    ASSERT_GE(raw.size(), 10u);
    for (size_t i = 0; i < 5; ++i) {
        EXPECT_EQ(raw[i], out[i]);
        EXPECT_EQ(raw[i + 5], out[i]);
    }
    in.close();
    removeTestFile(kTestFileName);
    recordLastError("FileIOTest::BinaryWriteAndRead");
}

TEST_F(FileIOTest, AppendMode) {
    removeTestFile(kTestFileName);
    {
        File file(kTestFileName, WriteOnly);
        ASSERT_TRUE(file.isOpen());
        ASSERT_TRUE(file.write("AAA"));
        file.close();
    }
    {
        // Append only takes effect combined with WriteOnly/ReadWrite mode
        File file(kTestFileName, WriteOnly | Append);
        ASSERT_TRUE(file.isOpen());
        ASSERT_TRUE(file.write("BBB"));
        file.close();
    }
    File file(kTestFileName, ReadOnly);
    ASSERT_TRUE(file.isOpen());
    EXPECT_EQ(file.readAllText(), std::string("AAABBB"));
    file.close();
    removeTestFile(kTestFileName);
}

TEST_F(FileIOTest, WriteAfterReopenOverwrites) {
    removeTestFile(kTestFileName);
    {
        File file(kTestFileName, WriteOnly);
        ASSERT_TRUE(file.isOpen());
        ASSERT_TRUE(file.write("old content that is long"));
        file.close();
    }
    {
        File file(kTestFileName, WriteOnly);
        ASSERT_TRUE(file.isOpen());
        ASSERT_TRUE(file.write("new"));
        file.close();
    }
    File file(kTestFileName, ReadOnly);
    ASSERT_TRUE(file.isOpen());
    auto all = file.readAllText();
    // OPEN_ALWAYS keeps old tail bytes when new content is shorter
    EXPECT_EQ(all.substr(0, 3), std::string("new"));
    file.close();
    removeTestFile(kTestFileName);
}

TEST_F(FileIOTest, WriteLineAndReadBack) {
    removeTestFile(kTestFileName);
    {
        File file(kTestFileName, WriteOnly);
        ASSERT_TRUE(file.isOpen());
        ASSERT_TRUE(file.writeLine("first"));
        ASSERT_TRUE(file.writeLine("second"));
        file.close();
    }
    File file(kTestFileName, ReadOnly);
    ASSERT_TRUE(file.isOpen());
    auto first = file.readLine();
    // writeLine appends "\r\n"; readLine strips the trailing "\n"
    ASSERT_FALSE(first.empty());
    EXPECT_EQ(first.substr(0, 5), std::string("first"));
    auto second = file.readLine();
    ASSERT_FALSE(second.empty());
    EXPECT_EQ(second.substr(0, 6), std::string("second"));
    file.close();
    removeTestFile(kTestFileName);
}

// ============================================================================
// File stream operators Tests
// ============================================================================

TEST(FileStreamOperatorTest, OutputOperator) {
    removeTestFile(kTestFileName);
    {
        File file(kTestFileName, WriteOnly);
        ASSERT_TRUE(file.isOpen());
        file << "Hello";
        file << " ";
        file << std::string("Stream");
        FileData data;
        data.push_back('!');
        file << data;
        file.close();
    }
    File file(kTestFileName, ReadOnly);
    ASSERT_TRUE(file.isOpen());
    EXPECT_EQ(file.readAllText(), std::string("Hello Stream!"));
    file.close();
    removeTestFile(kTestFileName);
}

TEST(FileStreamOperatorTest, ExtractOperator) {
    removeTestFile(kTestFileName);
    {
        File writer(kTestFileName, WriteOnly);
        ASSERT_TRUE(writer.isOpen());
        ASSERT_TRUE(writer.write(std::string(kTestContent)));
        writer.close();
    }

    File file(kTestFileName, ReadOnly);
    ASSERT_TRUE(file.isOpen());

    std::string word;
    file > word;
    EXPECT_EQ(word, std::string("Hello"));

    // The string extractor consumes the trailing whitespace separator
    char ch = '\0';
    file > ch;
    EXPECT_EQ(ch, 'T');

    file > word;
    EXPECT_EQ(word, std::string("iny!"));

    file.close();
    removeTestFile(kTestFileName);
}

TEST(FileStreamOperatorTest, ExtractRestOperators) {
    removeTestFile(kTestFileName);
    {
        File writer(kTestFileName, WriteOnly);
        ASSERT_TRUE(writer.isOpen());
        ASSERT_TRUE(writer.write(std::string(kTestContent)));
        writer.close();
    }

    File file(kTestFileName, ReadOnly);
    ASSERT_TRUE(file.isOpen());

    char ch = '\0';
    file > ch;
    EXPECT_EQ(ch, 'H');

    uint8_t byte = 0;
    file > byte;
    EXPECT_EQ(byte, static_cast<uint8_t>('e'));

    file.moveToStart();
    std::string all;
    file >> all;
    EXPECT_EQ(all, std::string(kTestContent));

    file.moveToStart();
    FileData raw;
    file >> raw;
    ASSERT_GE(raw.size(), contentLen());
    std::string content(raw.begin(), raw.begin() + static_cast<long>(contentLen()));
    EXPECT_EQ(content, std::string(kTestContent));

    file.close();
    removeTestFile(kTestFileName);
}

// ============================================================================
// File path/name Tests
// ============================================================================

TEST(FilePathTest, PathAndFileName) {
    removeTestFile(kTestFileName);
    File file(kTestFileName, WriteOnly);
    ASSERT_TRUE(file.isOpen());
    EXPECT_NE(file.path().find(kTestFileName), std::string::npos);
    EXPECT_EQ(file.fileName(), std::string(kTestFileName));
    file.close();
    removeTestFile(kTestFileName);
}

// ============================================================================
// File move semantics Tests
// ============================================================================

TEST(FileMoveTest, MoveConstructor) {
    const char* name = "tiny_test_move_ctor.tmp";
    removeTestFile(name);
    File src(name, WriteOnly);
    ASSERT_TRUE(src.isOpen());
    ASSERT_TRUE(src.write("moved"));
    auto src_path = src.path();

    File dst(std::move(src));
    EXPECT_TRUE(dst.isOpen());
    EXPECT_EQ(dst.path(), src_path);
    EXPECT_TRUE(dst.write("!"));
    dst.close();

    File check(name, ReadOnly);
    ASSERT_TRUE(check.isOpen());
    EXPECT_EQ(check.readAllText(), std::string("moved!"));
    check.close();
    removeTestFile(name);
}

TEST(FileMoveTest, MoveAssignment) {
    const char* name = "tiny_test_move_assign.tmp";
    removeTestFile(name);
    File src(name, WriteOnly);
    ASSERT_TRUE(src.isOpen());
    ASSERT_TRUE(src.write("assigned"));

    File dst("no_such_file_or_dir_tiny_test_xyz", ReadOnly);
    dst = std::move(src);
    EXPECT_TRUE(dst.isOpen());
    EXPECT_TRUE(dst.write(" ok"));
    dst.close();

    File check(name, ReadOnly);
    ASSERT_TRUE(check.isOpen());
    EXPECT_EQ(check.readAllText(), std::string("assigned ok"));
    check.close();
    removeTestFile(name);
}

// ============================================================================
// main
// ============================================================================
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
