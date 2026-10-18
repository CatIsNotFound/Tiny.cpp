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
#include <cstdio>
#include <string>
#include <vector>

#ifdef TINY_CPP_MY_OS_UNIX
#include <sys/stat.h>
#endif

using namespace Tiny::OS;

// ============================================================================
// Test helpers
// ============================================================================

/// Build an absolute path under the current working directory.
/// NOTE: never feed bare relative paths to FileSystem::mkDir — if any ancestor
/// does not exist, mkDirCompletely() walks up with Path::upper() and can loop
/// forever on a separator-less path, exhausting memory. Absolute paths always
/// terminate the walk at an existing ancestor.
static std::string absPath(const std::string& rel) {
    Path base = FileSystem::currentPath();
    base.join(rel);
    return base.path();
}

static std::string testDir() { return absPath("tiny_sys_dir"); }
static std::string testCopyDir() { return absPath("tiny_sys_dir_copy"); }

/// Record the last system error info (project convention for system-related operations)
static void recordLastError(const char* context) {
    std::string info;
    int code = 0;
    lastSystemError(info, &code);
    SUCCEED() << context << " | last system error code: " << code
              << ", info: " << (info.empty() ? "<none>" : info);
}

static void cleanupTestDirs() {
    FileSystem::rmDir(testCopyDir(), true);
    FileSystem::rmDir(testDir(), true);
}

static bool containsShortName(const std::vector<Path>& paths, const std::string& name) {
    for (auto& path : paths) {
        if (path.shortFileName() == name) return true;
    }
    return false;
}

// ============================================================================
// OS name Tests
// ============================================================================

TEST(OsNameTest, PlatformName) {
#if defined(_WIN32) || defined(_WIN64)
    EXPECT_STREQ(Name, "windows");
#else
    EXPECT_STRNE(Name, "");
#endif
}

// ============================================================================
// CPU architecture Tests
// ============================================================================

TEST(CPUArchTest, ArchNames) {
    EXPECT_STREQ(getCPUArchName(CPU_Arch::Unknown), "Unknown");
    EXPECT_STREQ(getCPUArchName(CPU_Arch::X86), "x86");
    EXPECT_STREQ(getCPUArchName(CPU_Arch::X86_64), "x86_64");
    EXPECT_STREQ(getCPUArchName(CPU_Arch::ARM32), "arm32");
    EXPECT_STREQ(getCPUArchName(CPU_Arch::ARM64), "arm64");
    EXPECT_STREQ(getCPUArchName(CPU_Arch::LoongArch), "loongArch");
    EXPECT_STREQ(getCPUArchName(CPU_Arch::MIPS), "mips");
    EXPECT_STREQ(getCPUArchName(CPU_Arch::RISCV), "risc-v");
    EXPECT_STREQ(getCPUArchName(CPU_Arch::IA64), "ia64");
}

TEST(CPUArchTest, CurrentArch) {
    auto arch = getCurrentCPUArch();
    EXPECT_NE(arch, CPU_Arch::Unknown);
    EXPECT_STRNE(getCPUArchName(arch), "Unknown");
}

// ============================================================================
// Host info Tests
// ============================================================================

TEST(HostInfoTest, GetHostInfo) {
    HostInfo info;
    bool ok = getHostInfo(info);
    EXPECT_TRUE(ok);
    ASSERT_FALSE(info.host_name.empty());
    EXPECT_NE(info.host_name[0], '\0');
    EXPECT_FALSE(info.user_name.empty());
    recordLastError("HostInfoTest::GetHostInfo");
}

TEST(HostInfoTest, CurrentHostInfo) {
    auto info = currentHostInfo();
    ASSERT_FALSE(info.host_name.empty());
    EXPECT_NE(info.host_name[0], '\0');
    EXPECT_FALSE(info.user_name.empty());
}

// ============================================================================
// CPU info Tests
// ============================================================================

TEST(CPUInfoTest, GetCPUInfo) {
    CPU info;
    // The PDH counters may need a warm-up call on some systems before the
    // formatted values become valid, so allow a few attempts.
    bool ok = false;
    for (int attempt = 0; attempt < 3 && !ok; ++attempt) {
        info = CPU{};
        ok = getCPUInfo(info, 50);
    }
    recordLastError("CPUInfoTest::GetCPUInfo");
    EXPECT_GT(info.cores, 0u);
    EXPECT_GT(info.page_size, 0u);
    EXPECT_EQ(info.usages.size(), static_cast<size_t>(info.cores));
    for (auto usage : info.usages) {
        EXPECT_GE(usage, 0.0f);
        EXPECT_LE(usage, 100.5f);
    }
    EXPECT_GE(info.total_usage, 0.0f);
    EXPECT_LE(info.total_usage, 100.5f);
    EXPECT_NE(info.machine, CPU_Arch::Unknown);
    if (!ok) {
        GTEST_SKIP() << "getCPUInfo counter sampling is unavailable in this environment";
    }
}

TEST(CPUInfoTest, CurrentCPUInfo) {
    auto info = currentCPUInfo();
    EXPECT_GT(info.cores, 0u);
    EXPECT_EQ(info.usages.size(), static_cast<size_t>(info.cores));
}

// ============================================================================
// Memory Tests
// ============================================================================

TEST(MemoryTest, GetMemory) {
    Memory memory;
    bool ok = getMemory(memory);
    EXPECT_TRUE(ok);
    EXPECT_GT(memory.total_ram, 0u);
    EXPECT_LE(memory.free_ram, memory.total_ram);
    EXPECT_LE(memory.used_ram, memory.total_ram);
    recordLastError("MemoryTest::GetMemory");
}

TEST(MemoryTest, CurrentMemory) {
    auto memory = currentMemory();
    EXPECT_GT(memory.total_ram, 0u);
    EXPECT_LE(memory.free_ram, memory.total_ram);
}

// ============================================================================
// Disk space Tests
// ============================================================================

TEST(DiskSpaceTest, GetDiskSpace) {
    DiskSpace space;
    bool ok = getDiskSpace(space);
    EXPECT_TRUE(ok);
    EXPECT_GT(space.total_bytes, 0u);
    EXPECT_LE(space.free_bytes, space.total_bytes);
    EXPECT_LE(space.used_bytes, space.total_bytes);
    recordLastError("DiskSpaceTest::GetDiskSpace");
}

TEST(DiskSpaceTest, CurrentDiskSpace) {
    auto space = currentDiskSpace();
    EXPECT_GT(space.total_bytes, 0u);
}

// ============================================================================
// Admin & last system error Tests
// ============================================================================

TEST(SystemMiscTest, IsAdmin) {
    bool admin = isAdmin();
    SUCCEED() << "isAdmin(): " << (admin ? "true" : "false");
}

TEST(SystemMiscTest, LastSystemError) {
    std::string info;
    int code = -1;
    lastSystemError(info, &code);
    EXPECT_GE(code, 0);
    if (code == 0) {
        EXPECT_TRUE(info.empty());
    }
    SUCCEED() << "lastSystemError code: " << code
              << ", info: " << (info.empty() ? "<none>" : info);
}

// ============================================================================
// Process::exec Tests
// ============================================================================

// Process::exec launches the program directly (CreateProcess, no shell), so
// shell builtins like "echo"/"exit" must run through the system shell.
#ifdef _WIN32
static const char* kEchoCmd = "cmd /c echo tiny-echo-test";
static const char* kExitCmd = "cmd /c exit 7";
static const char* kEchoCmd2 = "cmd /c echo deprecated-wrapper";
#else
static const char* kEchoCmd = "echo tiny-echo-test";
static const char* kExitCmd = "sh -c \"exit 7\"";
static const char* kEchoCmd2 = "echo deprecated-wrapper";
#endif

TEST(ProcessExecTest, EchoOutput) {
    std::string output, error;
    // NOTE: any nonzero timeout makes exec() kill the job object afterwards,
    // which can overwrite the real exit code with 255, so wait indefinitely
    int code = Process::exec(kEchoCmd, 0, &output, &error);
    EXPECT_EQ(code, 0);
    EXPECT_NE(output.find("tiny-echo-test"), std::string::npos) << "output: " << output;
    recordLastError("ProcessExecTest::EchoOutput");
}

TEST(ProcessExecTest, ExitCode) {
    int code = Process::exec(kExitCmd, 0);
    EXPECT_EQ(code, 7);
}

TEST(ProcessExecTest, EmptyCommand) {
    int code = Process::exec("", 10000);
    EXPECT_EQ(code, -1);
}

TEST(ProcessExecTest, NonExistentCommand) {
    int code = Process::exec("this_command_does_not_exist_xyz", 10000);
    EXPECT_NE(code, 0);
    recordLastError("ProcessExecTest::NonExistentCommand");
}

TEST(ProcessExecTest, DeprecatedWrapper) {
    std::string output;
    int code = exec(kEchoCmd2, 0, &output);
    EXPECT_EQ(code, 0);
    EXPECT_NE(output.find("deprecated-wrapper"), std::string::npos) << "output: " << output;
}

// ============================================================================
// Process::Child Tests
// ============================================================================

TEST(ProcessChildTest, RunAndCaptureOutput) {
    Process::Child child;
    ASSERT_TRUE(child.exec("echo child-ok"));

    bool ok = false;
    auto output = child.catchOutput(5000, &ok);
    EXPECT_TRUE(ok);
    EXPECT_NE(output.find("child-ok"), std::string::npos) << "output: " << output;

    int code = child.wait();
    EXPECT_EQ(code, 0);
    EXPECT_FALSE(child.running());
    EXPECT_EQ(child.exitCode(), 0);
}

TEST(ProcessChildTest, ChildExitCode) {
    Process::Child child;
    ASSERT_TRUE(child.exec("exit 3"));
    int code = child.wait();
    EXPECT_EQ(code, 3);
    EXPECT_EQ(child.exitCode(), 3);
}

TEST(ProcessChildTest, WithoutCatchMode) {
    Process::Child child(false, false);
    ASSERT_TRUE(child.exec("echo no-catch"));
    int code = child.wait();
    EXPECT_EQ(code, 0);

    auto output = child.output();
    EXPECT_TRUE(output.empty());
    recordLastError("ProcessChildTest::WithoutCatchMode");
}

// ============================================================================
// FileSystem basic Tests
// ============================================================================

TEST(FileSystemPathTest, CurrentAndHomePath) {
    auto current = FileSystem::currentPath();
    EXPECT_TRUE(current.isValid());
    EXPECT_TRUE(current.isDirectory());

    auto home = FileSystem::homePath();
    EXPECT_TRUE(home.isValid());
    EXPECT_TRUE(home.isDirectory());
    EXPECT_FALSE(home.path().empty());

    auto cache = FileSystem::cachePath();
    EXPECT_TRUE(cache.isValid());
    EXPECT_FALSE(cache.path().empty());

    auto local_data = FileSystem::localDataPath();
    EXPECT_TRUE(local_data.isValid());
    EXPECT_FALSE(local_data.path().empty());
    recordLastError("FileSystemPathTest::CurrentAndHomePath");
}

TEST(FileSystemPathTest, ChDir) {
    auto current = FileSystem::currentPath();
    ASSERT_TRUE(current.isValid());
    auto home = FileSystem::homePath();

    EXPECT_TRUE(FileSystem::chDir(home));
    EXPECT_EQ(FileSystem::currentPath().path(), home.path());

    EXPECT_TRUE(FileSystem::chDir(current));
    EXPECT_EQ(FileSystem::currentPath().path(), current.path());
    recordLastError("FileSystemPathTest::ChDir");
}

TEST(FileSystemPathTest, ChDirNonExistent) {
    EXPECT_FALSE(FileSystem::chDir(std::string("no_such_dir_tiny_test_xyz")));
    recordLastError("FileSystemPathTest::ChDirNonExistent");
}

// ============================================================================
// FileSystem create/copy/move/remove Tests
// ============================================================================

class FileSystemOpsTest : public ::testing::Test {
protected:
    void SetUp() override {
        cleanupTestDirs();
        recordLastError("FileSystemOpsTest::SetUp cleanup");
    }
    void TearDown() override {
        cleanupTestDirs();
        recordLastError("FileSystemOpsTest::TearDown cleanup");
    }
};

TEST_F(FileSystemOpsTest, MkDir) {
    EXPECT_TRUE(FileSystem::mkDir(testDir()));
    EXPECT_TRUE(Path::exist(testDir()));
    EXPECT_TRUE(Path::isDirectory(testDir()));
    // Creating an existing directory succeeds
    EXPECT_TRUE(FileSystem::mkDir(testDir()));

    // Nested creation one level below an existing directory
    EXPECT_TRUE(FileSystem::mkDir(testDir() + "/l1"));
    EXPECT_TRUE(Path::isDirectory(testDir() + "/l1"));
}

TEST_F(FileSystemOpsTest, MkDirInvalid) {
    // Creating a directory under an existing file must fail on every platform
    std::string file_path = absPath("tiny_not_a_dir.txt");
    ASSERT_TRUE(FileSystem::mkFile(file_path, std::string("x")));
    ASSERT_TRUE(Path::isFile(file_path));

    // The walk-up inside mkDir stops at the existing file ancestor, then the
    // native CreateDirectory call fails because the parent is not a directory
    EXPECT_FALSE(FileSystem::mkDir(file_path + "/subdir"));
    EXPECT_FALSE(Path::exist(file_path + "/subdir"));

    EXPECT_TRUE(FileSystem::rmFile(Path(file_path)));
    recordLastError("FileSystemOpsTest::MkDirInvalid");
}

TEST_F(FileSystemOpsTest, MkFileWithText) {
    ASSERT_TRUE(FileSystem::mkDir(testDir()));
    std::string file_path = testDir() + "/a.txt";
    EXPECT_TRUE(FileSystem::mkFile(file_path, std::string("AAA")));
    EXPECT_TRUE(Path::exist(file_path));
    EXPECT_TRUE(Path::isFile(file_path));
}

TEST_F(FileSystemOpsTest, MkFileWithBytes) {
    ASSERT_TRUE(FileSystem::mkDir(testDir()));
    std::string file_path = testDir() + "/b.bin";
    std::vector<uint8_t> data;
    data.push_back(0x00);
    data.push_back(0x10);
    data.push_back(0xFF);
    EXPECT_TRUE(FileSystem::mkFile(file_path, data));

    File file(Path(file_path), ReadOnly);
    ASSERT_TRUE(file.open(ReadOnly));
    auto raw = file.readAll();
    ASSERT_GE(raw.size(), 3u);
    EXPECT_EQ(raw[0], 0x00);
    EXPECT_EQ(raw[1], 0x10);
    EXPECT_EQ(raw[2], 0xFF);
    file.close();
    recordLastError("FileSystemOpsTest::MkFileWithBytes");
}

TEST_F(FileSystemOpsTest, CpFileMvFileRmFile) {
    ASSERT_TRUE(FileSystem::mkDir(testDir()));
    std::string src = testDir() + "/a.txt";
    std::string copied = testDir() + "/copy.txt";
    std::string moved = testDir() + "/moved.txt";
    ASSERT_TRUE(FileSystem::mkFile(src, std::string("copy me")));

    // Copy
    EXPECT_TRUE(FileSystem::cpFile(Path(src), Path(copied)));
    EXPECT_TRUE(Path::exist(copied));

    // Move
    EXPECT_TRUE(FileSystem::mvFile(Path(copied), Path(moved)));
    EXPECT_FALSE(Path::exist(copied));
    EXPECT_TRUE(Path::exist(moved));

    // Remove
    EXPECT_TRUE(FileSystem::rmFile(Path(moved)));
    EXPECT_FALSE(Path::exist(moved));
    recordLastError("FileSystemOpsTest::CpFileMvFileRmFile");
}

TEST_F(FileSystemOpsTest, CpFileStringOverloads) {
    ASSERT_TRUE(FileSystem::mkDir(testDir()));
    std::string src = testDir() + "/a.txt";
    std::string copied = testDir() + "/copy.txt";
    ASSERT_TRUE(FileSystem::mkFile(src, std::string("overload")));

    EXPECT_TRUE(FileSystem::cpFile(Path(src), copied));
    EXPECT_TRUE(Path::exist(copied));

    EXPECT_TRUE(FileSystem::mvFile(Path(copied), testDir() + "/renamed.txt"));
    EXPECT_TRUE(Path::exist(testDir() + "/renamed.txt"));
}

TEST_F(FileSystemOpsTest, CpDirMvDirRmDir) {
    ASSERT_TRUE(FileSystem::mkDir(testDir()));
    ASSERT_TRUE(FileSystem::mkFile(testDir() + "/a.txt", std::string("dir copy")));

    // Copy directory (creates the destination directory)
    EXPECT_TRUE(FileSystem::cpDir(Path(testDir()), Path(testCopyDir())));
    EXPECT_TRUE(Path::isDirectory(testCopyDir()));

    // Move directory
    std::string moved_dir = testCopyDir() + "_moved";
    EXPECT_TRUE(FileSystem::mvDir(Path(testCopyDir()), Path(moved_dir)));
    EXPECT_FALSE(Path::exist(testCopyDir()));
    EXPECT_TRUE(Path::isDirectory(moved_dir));

    // Remove recursively succeeds
    EXPECT_TRUE(FileSystem::rmDir(Path(moved_dir), true));
    EXPECT_FALSE(Path::exist(moved_dir));
    recordLastError("FileSystemOpsTest::CpDirMvDirRmDir");
}

TEST_F(FileSystemOpsTest, RmDirNonRecursiveOnNonEmpty) {
    // Use a dedicated directory name to avoid delete/recreate races on the
    // shared test directory across tests
    const std::string dir = absPath("tiny_sys_dir_rm");
    FileSystem::rmDir(dir, true);

    ASSERT_TRUE(FileSystem::mkDir(dir));
    ASSERT_TRUE(FileSystem::mkFile(dir + "/f.txt", std::string("x")));

    EXPECT_FALSE(FileSystem::rmDir(Path(dir), false));
    EXPECT_TRUE(Path::exist(dir));
    recordLastError("FileSystemOpsTest::RmDirNonRecursiveOnNonEmpty");

    EXPECT_TRUE(FileSystem::rmDir(Path(dir), true));
    EXPECT_FALSE(Path::exist(dir));
}

TEST_F(FileSystemOpsTest, RmDirNonExistent) {
    EXPECT_FALSE(FileSystem::rmDir(Path("no_such_dir_tiny_test_xyz"), true));
    EXPECT_FALSE(FileSystem::rmFile(Path("no_such_file_tiny_test_xyz.txt")));
    recordLastError("FileSystemOpsTest::RmDirNonExistent");
}

TEST_F(FileSystemOpsTest, MkLink) {
    ASSERT_TRUE(FileSystem::mkDir(testDir()));
    std::string target = testDir() + "/a.txt";
    ASSERT_TRUE(FileSystem::mkFile(target, std::string("link target")));
    std::string link = testDir() + "/a_link.txt";

    if (FileSystem::mkLink(target, link)) {
        EXPECT_TRUE(Path::exist(link));
        FileSystem::rmFile(Path(link));
    } else {
        // Creating a symbolic link requires additional privileges on some systems
        SUCCEED() << "mkLink is not permitted in current environment";
        recordLastError("FileSystemOpsTest::MkLink");
    }
}

// ============================================================================
// FileSystem listPath Tests
// ============================================================================

class FileSystemListTest : public ::testing::Test {
protected:
    void SetUp() override {
        cleanupTestDirs();
        ASSERT_TRUE(FileSystem::mkDir(testDir()));
        ASSERT_TRUE(FileSystem::mkDir(testDir() + "/sub"));
        ASSERT_TRUE(FileSystem::mkFile(testDir() + "/a.txt", std::string("A")));
        ASSERT_TRUE(FileSystem::mkFile(testDir() + "/b.log", std::string("B")));
        ASSERT_TRUE(FileSystem::mkFile(testDir() + "/sub/c.txt", std::string("C")));
    }
    void TearDown() override {
        cleanupTestDirs();
    }
};

TEST_F(FileSystemListTest, ListTopLevel) {
    auto paths = FileSystem::listPath(Path(testDir()), 1);
    EXPECT_TRUE(containsShortName(paths, "a.txt"));
    EXPECT_TRUE(containsShortName(paths, "b.log"));
    EXPECT_TRUE(containsShortName(paths, "sub"));
    // recursion_count = 1 does not contain nested entries
    EXPECT_FALSE(containsShortName(paths, "c.txt"));
}

TEST_F(FileSystemListTest, ListRecursive) {
    auto paths = FileSystem::listPath(Path(testDir()), 2);
    EXPECT_TRUE(containsShortName(paths, "a.txt"));
    EXPECT_TRUE(containsShortName(paths, "c.txt"));
}

TEST_F(FileSystemListTest, ListWithFilter) {
    auto paths = FileSystem::listPath(Path(testDir()), 1,
        [](const Path& path) {
            return path.shortFileName().find(".txt") != std::string::npos;
        });
    EXPECT_TRUE(containsShortName(paths, "a.txt"));
    EXPECT_FALSE(containsShortName(paths, "b.log"));
}

TEST_F(FileSystemListTest, ListByString) {
    auto paths = FileSystem::listPath(testDir(), 1);
    EXPECT_TRUE(containsShortName(paths, "a.txt"));
}

TEST_F(FileSystemListTest, ListPathEx) {
    auto layers = FileSystem::listPathEx(Path(testDir()), 1);
    EXPECT_GT(layers.size(), 0u);
    bool found = false;
    auto it = layers.find(1);
    if (it != layers.end()) {
        found = containsShortName(it->second, "a.txt");
    }
    EXPECT_TRUE(found);

    // found_event can stop the traversal
    FileSystem::LayerMap stopped;
    stopped = FileSystem::listPathEx(Path(testDir()), 2,
        [](const Path&, bool& stop) {
            stop = true;
            return true;
        });
    EXPECT_GE(stopped.size(), 1u);
}

// ============================================================================
// FileSnapShot Tests
// ============================================================================

// The Trigger enum is a public bitmask contract: Metadata aggregates the five
// metadata triggers, and All adds Context on top of it.
TEST(FileSnapShotTriggerTest, BitmaskValues) {
    EXPECT_EQ(static_cast<uint8_t>(FileSnapShot::None), 0u);
    EXPECT_EQ(static_cast<uint8_t>(FileSnapShot::Filename), 1u);
    EXPECT_EQ(static_cast<uint8_t>(FileSnapShot::Filesize), 2u);
    EXPECT_EQ(static_cast<uint8_t>(FileSnapShot::Permission), 4u);
    EXPECT_EQ(static_cast<uint8_t>(FileSnapShot::Timestamp), 8u);
    EXPECT_EQ(static_cast<uint8_t>(FileSnapShot::Owner), 16u);
    EXPECT_EQ(static_cast<uint8_t>(FileSnapShot::Metadata), 31u);
    EXPECT_EQ(static_cast<uint8_t>(FileSnapShot::Context), 32u);
    EXPECT_EQ(static_cast<uint8_t>(FileSnapShot::All), 63u);
}

TEST(FileSnapShotTriggerTest, DefaultAndSetter) {
    FileSnapShot snap;
    EXPECT_EQ(snap.trigger(), static_cast<uint8_t>(FileSnapShot::None));

    snap.setTrigger(FileSnapShot::Filesize);
    EXPECT_EQ(snap.trigger(), static_cast<uint8_t>(FileSnapShot::Filesize));

    auto combined = static_cast<FileSnapShot::Trigger>(
        FileSnapShot::Timestamp | FileSnapShot::Filesize);
    snap.setTrigger(combined);
    EXPECT_EQ(snap.trigger(), static_cast<uint8_t>(combined));

    snap.setTrigger(FileSnapShot::All);
    EXPECT_EQ(snap.trigger(), static_cast<uint8_t>(FileSnapShot::All));
}

class FileSnapShotTest : public ::testing::Test {
protected:
    void SetUp() override {
        FileSystem::rmFile(snapFile());
        ASSERT_TRUE(FileSystem::mkFile(snapFile(), std::string("snapshot")));
        ASSERT_TRUE(Path(snapFile()).isValid());
    }
    void TearDown() override {
        FileSystem::rmFile(snapFile());
        FileSystem::rmDir(snapDir(), true);
    }

    // mkFile opens with O_EXCL (CREATE_ALWAYS is only exclusive for existing
    // handles on POSIX), so an existing file must be removed before it can be
    // recreated with new content.
    void rewriteFile(const std::string& content) {
        EXPECT_TRUE(FileSystem::rmFile(snapFile()));
        EXPECT_TRUE(FileSystem::mkFile(snapFile(), content));
    }

    static std::string snapFile() { return absPath("tiny_snapshot_file.tmp"); }
    static std::string snapDir() { return absPath("tiny_snapshot_dir"); }
};

// NOTE on the detection pattern used below: the first check() call captures
// the baseline ("before") snapshot, the file is then modified, and the second
// check() call compares against that baseline and reports the change. This is
// deterministic and needs no background writer threads. check() blocks for
// roughly check_delay_ms per round.

TEST_F(FileSnapShotTest, CheckWithoutWatchedPath) {
    FileSnapShot snap;
    snap.setTrigger(FileSnapShot::All);

    std::vector<uint64_t> result;
    EXPECT_FALSE(snap.check(1, 20, &result));
    EXPECT_TRUE(result.empty());
    recordLastError("FileSnapShotTest::CheckWithoutWatchedPath");
}

TEST_F(FileSnapShotTest, AddRemoveAndReAddPath) {
    Path path(snapFile());
    FileSnapShot snap;
    snap.setTrigger(FileSnapShot::Filesize);

    // All three removePath overloads drop the watched path again
    snap.addPath(path);
    snap.removePath(snapFile());
    snap.addPath(path);
    snap.removePath(&path);
    snap.addPath(path);
    snap.removePath(path);
    snap.clearAll();

    // Nothing is watched anymore, so no change can be reported
    std::vector<uint64_t> result;
    EXPECT_FALSE(snap.check(1, 20, &result));
    EXPECT_TRUE(result.empty());

    // Re-adding resumes detection on the same Path object
    snap.addPath(path);
    EXPECT_FALSE(snap.check(1, 20, &result));
    rewriteFile("a considerably longer content for the re-added snapshot path");
    EXPECT_TRUE(snap.check(1, 20, &result));
    ASSERT_FALSE(result.empty());
    EXPECT_EQ(result.front(), path.uniqueID());
}

TEST_F(FileSnapShotTest, NoChangeNoTrigger) {
    Path path(snapFile());
    FileSnapShot snap;
    snap.addPath(path);
    snap.setTrigger(FileSnapShot::All);

    // Two polling rounds over an untouched file must not report anything
    std::vector<uint64_t> result;
    EXPECT_FALSE(snap.check(2, 30, &result));
    EXPECT_TRUE(result.empty());
    recordLastError("FileSnapShotTest::NoChangeNoTrigger");
}

TEST_F(FileSnapShotTest, DetectFilesizeChange) {
    Path path(snapFile());
    FileSnapShot snap;
    snap.addPath(path);
    snap.setTrigger(FileSnapShot::Filesize);

    std::vector<uint64_t> result;
    EXPECT_FALSE(snap.check(1, 20, &result));  // baseline capture
    rewriteFile("a much longer content than the baseline snapshot");
    EXPECT_TRUE(snap.check(1, 20, &result));

    // The result holds the uniqueID() of the triggered watched path
    ASSERT_FALSE(result.empty());
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result.front(), path.uniqueID());
}

TEST_F(FileSnapShotTest, DetectTimestampChange) {
    Path path(snapFile());
    FileSnapShot snap;
    snap.addPath(path);
    snap.setTrigger(FileSnapShot::Timestamp);

    std::vector<uint64_t> result;
    EXPECT_FALSE(snap.check(1, 20, &result));  // baseline capture
    rewriteFile("rewritten after the baseline, so the write time differs");
    EXPECT_TRUE(snap.check(1, 20, &result));

    ASSERT_FALSE(result.empty());
    EXPECT_EQ(result.front(), path.uniqueID());
}

TEST_F(FileSnapShotTest, DetectPermissionChange) {
#if defined(_WIN32) || defined(_WIN64)
    GTEST_SKIP() << "POSIX permission bits are not meaningful on Windows";
#else
    Path path(snapFile());
    FileSnapShot snap;
    snap.addPath(path);
    snap.setTrigger(FileSnapShot::Permission);

    std::vector<uint64_t> result;
    EXPECT_FALSE(snap.check(1, 20, &result));  // baseline capture

    // Flip the group/other rwx bits so the new mode is guaranteed to differ
    struct stat st {};
    ASSERT_EQ(::stat(snapFile().c_str(), &st), 0);
    ASSERT_EQ(::chmod(snapFile().c_str(), (st.st_mode & 07777) ^ 0077), 0);

    EXPECT_TRUE(snap.check(1, 20, &result));
    ASSERT_FALSE(result.empty());
    EXPECT_EQ(result.front(), path.uniqueID());
    recordLastError("FileSnapShotTest::DetectPermissionChange");
#endif
}

TEST_F(FileSnapShotTest, DetectFileDeletion) {
    Path path(snapFile());
    FileSnapShot snap;
    snap.addPath(path);
    snap.setTrigger(FileSnapShot::Filesize);

    std::vector<uint64_t> result;
    EXPECT_FALSE(snap.check(1, 20, &result));  // baseline capture
    EXPECT_TRUE(FileSystem::rmFile(snapFile()));
    // A deleted file re-stats as "missing": its size drops to 0, which is a
    // detectable Filesize difference against the baseline
    EXPECT_TRUE(snap.check(1, 20, &result));

    ASSERT_FALSE(result.empty());
    EXPECT_EQ(result.front(), path.uniqueID());
}

TEST_F(FileSnapShotTest, DetectDirectoryChange) {
    ASSERT_TRUE(FileSystem::mkDir(snapDir()));

    Path dir_path(snapDir());
    FileSnapShot snap;
    snap.addPath(dir_path);
    snap.setTrigger(FileSnapShot::Timestamp);

    std::vector<uint64_t> result;
    EXPECT_FALSE(snap.check(1, 20, &result));  // baseline capture
    // Creating an entry updates the parent directory's modification time
    ASSERT_TRUE(FileSystem::mkFile(snapDir() + "/new_entry.txt", std::string("x")));
    EXPECT_TRUE(snap.check(1, 20, &result));

    ASSERT_FALSE(result.empty());
    EXPECT_EQ(result.front(), dir_path.uniqueID());

    FileSystem::rmDir(snapDir(), true);
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
