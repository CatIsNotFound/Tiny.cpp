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


TEST(FileSystemTest, CurrentPath) {
    Path current = FileSystem::currentPath();
    if (!current.isValid()) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "currentPath() failed: " << error;
    }
    EXPECT_TRUE(current.isValid());
    EXPECT_TRUE(current.isDirectory());
}

TEST(FileSystemTest, HomePath) {
    Path home = FileSystem::homePath();
    if (!home.isValid()) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "homePath() failed: " << error;
    }
    EXPECT_TRUE(home.isValid());
    EXPECT_TRUE(home.isDirectory());
}

TEST(FileSystemTest, ChDir_String) {
    Path originalDir = FileSystem::currentPath();
    if (!FileSystem::chDir("..")) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "chDir(\"..\") failed: " << error;
    }
    Path newDir = FileSystem::currentPath();
    EXPECT_TRUE(newDir.isValid());
    if (!FileSystem::chDir(originalDir.path())) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "chDir(originalDir) failed: " << error;
    }
    Path backDir = FileSystem::currentPath();
    EXPECT_EQ(backDir.path(), originalDir.path());
}

TEST(FileSystemTest, ChDir_Path) {
    Path originalDir = FileSystem::currentPath();
    Path parentDir("..");
    if (!FileSystem::chDir(parentDir)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "chDir(parentDir) failed: " << error;
    }
    Path newDir = FileSystem::currentPath();
    EXPECT_TRUE(newDir.isValid());
    if (!FileSystem::chDir(originalDir)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "chDir(originalDir) failed: " << error;
    }
    Path backDir = FileSystem::currentPath();
    EXPECT_EQ(backDir.path(), originalDir.path());
}

TEST(FileSystemTest, ChDir_Invalid) {
    bool result = FileSystem::chDir("/this/path/does/not/exist/12345");
    if (result) {
        ADD_FAILURE() << "chDir() should have failed for non-existent path";
    } else {
        std::string error;
        lastSystemError(error);
        EXPECT_FALSE(result) << "chDir() failed as expected: " << error;
    }
}

TEST(FileSystemTest, MkDirAndRmDir_String) {
    std::string testDir = "./test_dir_temp";
    Path checkPath(testDir);
    if (checkPath.isValid()) {
        if (!FileSystem::rmDir(testDir, true)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmDir() cleanup failed: " << error;
        }
    }

    if (!FileSystem::mkDir(testDir)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkDir() failed: " << error;
    }
    Path verifyPath(testDir);
    EXPECT_TRUE(verifyPath.isValid());
    EXPECT_TRUE(verifyPath.isDirectory());

    if (!FileSystem::rmDir(testDir)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmDir() failed: " << error;
    }
    Path removedPath(testDir);
    EXPECT_FALSE(removedPath.isValid());
}

TEST(FileSystemTest, MkDirAndRmDir_Path) {
    std::string testDir = "./test_dir_temp_path";
    Path checkPath(testDir);
    if (checkPath.isValid()) {
        if (!FileSystem::rmDir(checkPath, true)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmDir() cleanup failed: " << error;
        }
    }

    Path dirPath(testDir);
    if (!FileSystem::mkDir(dirPath)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkDir(Path) failed: " << error;
    }
    Path verifyPath(testDir);
    EXPECT_TRUE(verifyPath.isValid());
    EXPECT_TRUE(verifyPath.isDirectory());

    Path removePath(testDir);
    if (!FileSystem::rmDir(removePath)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmDir(Path) failed: " << error;
    }
    Path removedPath(testDir);
    EXPECT_FALSE(removedPath.isValid());
}

TEST(FileSystemTest, MkDir_Existing) {
    std::string testDir = "./test_existing_dir";
    Path checkPath(testDir);
    if (checkPath.isValid()) {
        if (!FileSystem::rmDir(testDir, true)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmDir() cleanup failed: " << error;
        }
    }

    if (!FileSystem::mkDir(testDir)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkDir() failed: " << error;
    }

    // Try to create it again - should fail or succeed depending on implementation
    bool secondResult = FileSystem::mkDir(testDir);
    // Either outcome is acceptable, just verify it doesn't crash

    if (!FileSystem::rmDir(testDir, true)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmDir() cleanup failed: " << error;
    }
}

TEST(FileSystemTest, RmDir_Recursion) {
    std::string parentDir = "./test_parent_dir";
    std::string childDir = parentDir + "/child_dir";
    std::string testFile = parentDir + "/test.txt";

    Path checkPath(parentDir);
    if (checkPath.isValid()) {
        if (!FileSystem::rmDir(parentDir, true)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmDir() cleanup failed: " << error;
        }
    }

    if (!FileSystem::mkDir(parentDir)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkDir(parentDir) failed: " << error;
    }
    if (!FileSystem::mkDir(childDir)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkDir(childDir) failed: " << error;
    }
    if (!FileSystem::mkFile(testFile, "test content")) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }
    if (!FileSystem::rmDir(parentDir, true)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmDir(recursive) failed: " << error;
    }
    Path removedPath(parentDir);
    EXPECT_FALSE(removedPath.isValid());
}

TEST(FileSystemTest, MkFileAndRmFile_String_Empty) {
    std::string testFile = "./test_file_empty.txt";
    Path checkPath(testFile);
    if (checkPath.isValid()) {
        if (!FileSystem::rmFile(testFile)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }

    if (!FileSystem::mkFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile() failed: " << error;
    }
    Path filePath(testFile);
    EXPECT_TRUE(filePath.isValid());
    EXPECT_TRUE(filePath.isFile());
    EXPECT_EQ(filePath.fileSize(), 0);

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() failed: " << error;
    }
    Path removedPath(testFile);
    EXPECT_FALSE(removedPath.isValid());
}

TEST(FileSystemTest, MkFileAndRmFile_Path_Empty) {
    std::string testFile = "./test_file_empty_path.txt";
    Path checkPath(testFile);
    if (checkPath.isValid()) {
        if (!FileSystem::rmFile(checkPath)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }

    Path filePath(testFile);
    if (!FileSystem::mkFile(filePath)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile(Path) failed: " << error;
    }
    Path verifyPath(testFile);
    EXPECT_TRUE(verifyPath.isValid());
    EXPECT_TRUE(verifyPath.isFile());

    Path removePath(testFile);
    if (!FileSystem::rmFile(removePath)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile(Path) failed: " << error;
    }
    Path removedPath(testFile);
    EXPECT_FALSE(removedPath.isValid());
}

TEST(FileSystemTest, MkFile_String_Vector) {
    std::string testFile = "./test_file_vector.bin";
    std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0x04, 0x05};

    Path checkPath(testFile);
    if (checkPath.isValid()) {
        if (!FileSystem::rmFile(testFile)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }

    if (!FileSystem::mkFile(testFile, data)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile(string, vector) failed: " << error;
    }
    Path filePath(testFile);
    EXPECT_TRUE(filePath.isValid());
    EXPECT_TRUE(filePath.isFile());
    EXPECT_EQ(filePath.fileSize(), data.size());

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() cleanup failed: " << error;
    }
}

TEST(FileSystemTest, MkFile_Path_Vector) {
    std::string testFile = "./test_file_vector_path.bin";
    std::vector<uint8_t> data = {0x0A, 0x0B, 0x0C};

    Path checkPath(testFile);
    if (checkPath.isValid()) {
        if (!FileSystem::rmFile(checkPath)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }

    Path filePath(testFile);
    if (!FileSystem::mkFile(filePath, data)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile(Path, vector) failed: " << error;
    }
    Path verifyPath(testFile);
    EXPECT_TRUE(verifyPath.isValid());
    EXPECT_EQ(verifyPath.fileSize(), data.size());

    if (!FileSystem::rmFile(filePath)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() cleanup failed: " << error;
    }
}

TEST(FileSystemTest, MkFile_String_String) {
    std::string testFile = "./test_file_string.txt";
    std::string content = "Hello, World!";

    Path checkPath(testFile);
    if (checkPath.isValid()) {
        if (!FileSystem::rmFile(testFile)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }

    if (!FileSystem::mkFile(testFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile(string, string) failed: " << error;
    }
    Path filePath(testFile);
    EXPECT_TRUE(filePath.isValid());
    EXPECT_TRUE(filePath.isFile());
    EXPECT_EQ(filePath.fileSize(), content.size());

    if (!FileSystem::rmFile(testFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() cleanup failed: " << error;
    }
}

TEST(FileSystemTest, MkFile_Path_String) {
    std::string testFile = "./test_file_string_path.txt";
    std::string content = "Test content with Path";

    Path checkPath(testFile);
    if (checkPath.isValid()) {
        if (!FileSystem::rmFile(checkPath)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }

    Path filePath(testFile);
    if (!FileSystem::mkFile(filePath, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile(Path, string) failed: " << error;
    }
    Path verifyPath(testFile);
    EXPECT_TRUE(verifyPath.isValid());
    EXPECT_EQ(verifyPath.fileSize(), content.size());

    if (!FileSystem::rmFile(filePath)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() cleanup failed: " << error;
    }
}

TEST(FileSystemTest, CpFile_PathPath) {
    std::string srcFile = "./test_cp_src.txt";
    std::string destFile = "./test_cp_dest.txt";
    std::string content = "Test content for copy";

    Path checkSrc(srcFile);
    if (checkSrc.isValid()) {
        if (!FileSystem::rmFile(srcFile)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }
    Path checkDest(destFile);
    if (checkDest.isValid()) {
        if (!FileSystem::rmFile(destFile)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }

    if (!FileSystem::mkFile(srcFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile(src) failed: " << error;
    }
    Path src(srcFile);
    Path dest(destFile);
    if (!FileSystem::cpFile(src, dest)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "cpFile(Path, Path) failed: " << error;
    }
    if (!FileSystem::cpFile(src, dest)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "FileSystem::cpFile() failed: " << error;
    }
    Path verifyDest(destFile);
    EXPECT_TRUE(verifyDest.isValid());
    EXPECT_TRUE(verifyDest.isFile());
    EXPECT_EQ(verifyDest.fileSize(), content.size());

    if (!FileSystem::rmFile(srcFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() cleanup failed: " << error;
    }
    if (!FileSystem::rmFile(destFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() cleanup failed: " << error;
    }
}

TEST(FileSystemTest, CpFile_PathString) {
    std::string srcFile = "./test_cp_src2.txt";
    std::string destFile = "./test_cp_dest2.txt";
    std::string content = "Test content for copy 2";

    Path checkSrc(srcFile);
    if (checkSrc.isValid()) {
        if (!FileSystem::rmFile(srcFile)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }
    Path checkDest(destFile);
    if (checkDest.isValid()) {
        if (!FileSystem::rmFile(destFile)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }

    if (!FileSystem::mkFile(srcFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile(src) failed: " << error;
    }
    Path src(srcFile);
    if (!FileSystem::cpFile(src, destFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "cpFile(Path, string) failed: " << error;
    }
    if (!FileSystem::cpFile(src, destFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "FileSystem::cpFile() failed: " << error;
    }
    Path verifyDest(destFile);
    EXPECT_TRUE(verifyDest.isValid());

    if (!FileSystem::rmFile(srcFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() cleanup failed: " << error;
    }
    if (!FileSystem::rmFile(destFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "rmFile() cleanup failed: " << error;
    }
}

TEST(FileSystemTest, MvFile_PathPath) {
    std::string srcFile = "./test_mv_src.txt";
    std::string destFile = "./test_mv_dest.txt";
    std::string content = "Test content for move";

    Path checkSrc(srcFile);
    if (checkSrc.isValid()) {
        if (!FileSystem::rmFile(srcFile)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }
    Path checkDest(destFile);
    if (checkDest.isValid()) {
        if (!FileSystem::rmFile(destFile)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }

    if (!FileSystem::mkFile(srcFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile(src) failed: " << error;
    }
    Path src(srcFile);
    Path dest(destFile);
    if (!FileSystem::mvFile(src, dest)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mvFile(Path, Path) failed: " << error;
    }

    if (Path(srcFile).isValid()) {
        if (!FileSystem::rmFile(srcFile)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }
    if (Path(destFile).isValid()) {
        if (!FileSystem::rmFile(destFile)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }
}

TEST(FileSystemTest, MvFile_PathString) {
    std::string srcFile = "./test_mv_src2.txt";
    std::string destFile = "./test_mv_dest2.txt";
    std::string content = "Test content for move 2";

    Path checkSrc(srcFile);
    if (checkSrc.isValid()) {
        if (!FileSystem::rmFile(srcFile)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }
    Path checkDest(destFile);
    if (checkDest.isValid()) {
        if (!FileSystem::rmFile(destFile)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }

    if (!FileSystem::mkFile(srcFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile(src) failed: " << error;
    }
    Path src(srcFile);
    if (!FileSystem::mvFile(src, destFile)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mvFile(Path, string) failed: " << error;
    }

    if (Path(srcFile).isValid()) {
        if (!FileSystem::rmFile(srcFile)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }
    if (Path(destFile).isValid()) {
        if (!FileSystem::rmFile(destFile)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }
}

TEST(FileSystemTest, CpDir_PathPath) {
    std::string srcDir = "./test_cpdir_src";
    std::string destDir = "./test_cpdir_dest";

    Path checkSrc(srcDir);
    if (checkSrc.isValid()) {
        if (!FileSystem::rmDir(srcDir, true)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmDir() cleanup failed: " << error;
        }
    }
    Path checkDest(destDir);
    if (checkDest.isValid()) {
        if (!FileSystem::rmDir(destDir, true)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmDir() cleanup failed: " << error;
        }
    }

    if (!FileSystem::mkDir(srcDir)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkDir(srcDir) failed: " << error;
    }
    if (!FileSystem::mkFile(srcDir + "/test.txt", "test content")) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile(srcDir/test.txt) failed: " << error;
    }
    Path src(srcDir);
    Path dest(destDir);
    if (!FileSystem::cpDir(src, dest)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "cpDir(Path, Path) failed: " << error;
    }

    if (Path(srcDir).isValid()) {
        if (!FileSystem::rmDir(srcDir, true)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmDir() cleanup failed: " << error;
        }
    }
    if (Path(destDir).isValid()) {
        if (!FileSystem::rmDir(destDir, true)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmDir() cleanup failed: " << error;
        }
    }
}

TEST(FileSystemTest, CpDir_PathString) {
    std::string srcDir = "./test_cpdir_src2";
    std::string destDir = "./test_cpdir_dest2";

    Path checkSrc(srcDir);
    if (checkSrc.isValid()) {
        if (!FileSystem::rmDir(srcDir, true)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmDir() cleanup failed: " << error;
        }
    }
    Path checkDest(destDir);
    if (checkDest.isValid()) {
        if (!FileSystem::rmDir(destDir, true)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmDir() cleanup failed: " << error;
        }
    }

    if (!FileSystem::mkDir(srcDir)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkDir(srcDir) failed: " << error;
    }
    if (!FileSystem::mkFile(srcDir + "/test.txt", "test content")) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile(srcDir/test.txt) failed: " << error;
    }
    Path src(srcDir);
    if (!FileSystem::cpDir(src, destDir)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "cpDir(Path, string) failed: " << error;
    }

    if (Path(srcDir).isValid()) {
        if (!FileSystem::rmDir(srcDir, true)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmDir() cleanup failed: " << error;
        }
    }
    if (Path(destDir).isValid()) {
        if (!FileSystem::rmDir(destDir, true)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmDir() cleanup failed: " << error;
        }
    }
}

TEST(FileSystemTest, MvDir_PathPath) {
    std::string srcDir = "./test_mvdir_src";
    std::string destDir = "./test_mvdir_dest";

    Path checkSrc(srcDir);
    if (checkSrc.isValid()) {
        if (!FileSystem::rmDir(srcDir, true)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmDir() cleanup failed: " << error;
        }
    }
    Path checkDest(destDir);
    if (checkDest.isValid()) {
        if (!FileSystem::rmDir(destDir, true)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmDir() cleanup failed: " << error;
        }
    }

    if (!FileSystem::mkDir(srcDir)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkDir(srcDir) failed: " << error;
    }
    Path src(srcDir);
    Path dest(destDir);
    if (!FileSystem::mvDir(src, dest)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mvDir(Path, Path) failed: " << error;
    }

    if (Path(srcDir).isValid()) {
        if (!FileSystem::rmDir(srcDir, true)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmDir() cleanup failed: " << error;
        }
    }
    if (Path(destDir).isValid()) {
        if (!FileSystem::rmDir(destDir, true)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmDir() cleanup failed: " << error;
        }
    }
}

TEST(FileSystemTest, MvDir_PathString) {
    std::string srcDir = "./test_mvdir_src2";
    std::string destDir = "./test_mvdir_dest2";

    Path checkSrc(srcDir);
    if (checkSrc.isValid()) {
        if (!FileSystem::rmDir(srcDir, true)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmDir() cleanup failed: " << error;
        }
    }
    Path checkDest(destDir);
    if (checkDest.isValid()) {
        if (!FileSystem::rmDir(destDir, true)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmDir() cleanup failed: " << error;
        }
    }

    if (!FileSystem::mkDir(srcDir)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkDir(srcDir) failed: " << error;
    }
    Path src(srcDir);
    if (!FileSystem::mvDir(src, destDir)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mvDir(Path, string) failed: " << error;
    }

    if (Path(srcDir).isValid()) {
        if (!FileSystem::rmDir(srcDir, true)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmDir() cleanup failed: " << error;
        }
    }
    if (Path(destDir).isValid()) {
        if (!FileSystem::rmDir(destDir, true)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmDir() cleanup failed: " << error;
        }
    }
}

TEST(FileSystemTest, MkLink_StringString) {
    std::string targetFile = "./test_link_target.txt";
    std::string linkFile = "./test_link.lnk";
    std::string content = "Test content for link";

    Path checkTarget(targetFile);
    if (checkTarget.isValid()) {
        if (!FileSystem::rmFile(targetFile)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }
    Path checkLink(linkFile);
    if (checkLink.isValid()) {
        if (!FileSystem::rmFile(linkFile)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }

    if (!FileSystem::mkFile(targetFile, content)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "mkFile(target) failed: " << error;
    }
    if (!FileSystem::mkLink(linkFile, targetFile)) {
#ifdef TINY_CPP_MY_OS_WINDOWS
        if (isAdmin()) {
#endif
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkLink(string, string) failed: " << error;
#ifdef TINY_CPP_MY_OS_WINDOWS
        }
#endif
    }

    if (Path(targetFile).isValid()) {
        if (!FileSystem::rmFile(targetFile)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }
    if (Path(linkFile).isValid()) {
        if (!FileSystem::rmFile(linkFile)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }
}

TEST(FileSystemTest, MkLink_StringPath) {
    std::string targetFile = "./test_link_target2.txt";
    std::string linkFile = "./test_link2.lnk";
    std::string content = "Test content for link 2";

    Path checkTarget(targetFile);
    if (checkTarget.isValid()) {
        if (!FileSystem::rmFile(targetFile)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }
    Path checkLink(linkFile);
    if (checkLink.isValid()) {
        if (!FileSystem::rmFile(linkFile)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }

    if (!FileSystem::mkFile(targetFile, content)) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "mkFile(target) failed: " << error;
    }
    Path target(targetFile);
    if (!FileSystem::mkLink(linkFile, target)) {
#ifdef TINY_CPP_MY_OS_WINDOWS
        if (isAdmin()) {
#endif
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "mkLink(string, Path) failed: " << error;
#ifdef TINY_CPP_MY_OS_WINDOWS
        }
#endif
    }

    if (Path(targetFile).isValid()) {
        if (!FileSystem::rmFile(targetFile)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }
    if (Path(linkFile).isValid()) {
        if (!FileSystem::rmFile(linkFile)) {
            std::string error;
            lastSystemError(error);
            ADD_FAILURE() << "rmFile() cleanup failed: " << error;
        }
    }
}

TEST(FileSystemTest, ListPath_String) {
    std::vector<Path> paths = FileSystem::listPath(".", 1);
    if (paths.empty()) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "listPath(string) returned empty: " << error;
    }
    EXPECT_FALSE(paths.empty());
}

TEST(FileSystemTest, ListPath_Path) {
    Path currentDir(".");
    std::vector<Path> paths = FileSystem::listPath(currentDir, 1);
    if (paths.empty()) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "listPath(Path) returned empty: " << error;
    }
    EXPECT_FALSE(paths.empty());
}

TEST(FileSystemTest, ListPath_NoArgs) {
    std::vector<Path> paths = FileSystem::listPath(1);
    if (paths.empty()) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "listPath() returned empty: " << error;
    }
    EXPECT_FALSE(paths.empty());
}

TEST(FileSystemTest, ListPath_WithFilter) {
    auto path = "./test_dir_for_listPath";
    FileSystem::mkDir(path);
    auto filter = [](const Path& p) { return p.isDirectory(); };
    std::vector<Path> paths = FileSystem::listPath(".", 1, filter);
    if (paths.empty()) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "listPath() with filter returned empty: " << error;
    }
    for (const auto& p : paths) {
        EXPECT_TRUE(p.isDirectory());
    }
    FileSystem::rmDir(path);
}

TEST(FileSystemTest, ListPath_Recursion) {
    std::vector<Path> paths = FileSystem::listPath(".", 2);
    if (paths.empty()) {
        std::string error;
        lastSystemError(error);
        ADD_FAILURE() << "listPath(recursive) returned empty: " << error;
    }
    EXPECT_FALSE(paths.empty());
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