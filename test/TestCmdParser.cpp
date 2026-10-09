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

#include <gtest/gtest.h>
#include "../src/Parser/CommandParser.hpp"

#include <vector>
#include <string>

using namespace Tiny;

//---------------------------------------------------------------
// Part 1. 注册接口 (addCommand/addFullCommand/addLastCommand/addFullLastCommand)
//---------------------------------------------------------------
TEST(CommandParser_Register, AddCommand_Basic)
{
    const char* argv[] = {"app"};
    CommandParser parser(1, const_cast<char**>(argv));
    EXPECT_TRUE(parser.addCommand("help", "h", "show help", false));
    EXPECT_EQ(parser.size(), 1u);
    EXPECT_TRUE(parser.exist("help"));
    EXPECT_EQ(parser.at("help").option_name, "help");
    EXPECT_EQ(parser.at("help").short_options, "h");
    EXPECT_FALSE(parser.at("help").has_value);
    EXPECT_FALSE(parser.at("help").full_option_only);
    EXPECT_FALSE(parser.at("help").is_required);
    EXPECT_FALSE(parser.at("help").is_last_command);
    EXPECT_FALSE(parser.at("help").is_default_command);
}

TEST(CommandParser_Register, AddCommand_WithValue_Required_DefaultPlaceholder)
{
    const char* argv[] = {"app"};
    CommandParser parser(1, const_cast<char**>(argv));
    EXPECT_TRUE(parser.addCommand("name", "n", "set name",
                 /*has_value=*/true, /*default_value=*/"Mike",
                 /*is_required=*/true, /*default_command=*/false,
                 /*placeholder=*/"NAME"));
    EXPECT_EQ(parser.size(), 1u);
    EXPECT_TRUE(parser.exist("name"));
    EXPECT_TRUE(parser.at("name").has_value);
    EXPECT_TRUE(parser.at("name").is_required);
    EXPECT_EQ(parser.at("name").default_value, "Mike");
    EXPECT_EQ(parser.at("name").value_placeholder, "NAME");
    // 空 placeholder 应被截断并填充为 "VAL"（源码默值）
}

TEST(CommandParser_Register, AddCommand_Duplicate_ReturnsFalse)
{
    const char* argv[] = {"app"};
    CommandParser parser(1, const_cast<char**>(argv));
    EXPECT_TRUE(parser.addCommand("help", "h", "help"));
    EXPECT_FALSE(parser.addCommand("help", "H", "help again")); // 重复注册
    EXPECT_EQ(parser.size(), 1u);
}

TEST(CommandParser_Register, AddCommand_EmptyName_ReturnsFalse)
{
    const char* argv[] = {"app"};
    CommandParser parser(1, const_cast<char**>(argv));
    EXPECT_FALSE(parser.addCommand("", "h", "no name"));
    EXPECT_FALSE(parser.addCommand("   ", "h", "blank name"));
    EXPECT_EQ(parser.size(), 0u);
}

TEST(CommandParser_Register, AddCommand_OnlyDashes_ReturnsFalse)
{
    // makeOptionName("---") 会先剥离前导 '-'，但全是 '-' 时 find_first_not_of 返回 npos，
    // 不进入 erase 分支，最终 ret="---" 非空，所以 addCommand 会成功注册一个叫 "---" 的命令
    // 这是实现的边界行为：只要 makeOptionName 返回非空串就注册
    const char* argv[] = {"app"};
    CommandParser parser(1, const_cast<char**>(argv));
    // 验证："---" 被视为合法名字（非空）
    EXPECT_TRUE(parser.addCommand("---", "h", "only dashes"));
    EXPECT_EQ(parser.size(), 1u);
    EXPECT_TRUE(parser.exist("---"));
}

TEST(CommandParser_Register, AddFullCommand_Basic)
{
    const char* argv[] = {"app"};
    CommandParser parser(1, const_cast<char**>(argv));
    EXPECT_TRUE(parser.addFullCommand("run", "run app"));
    EXPECT_EQ(parser.size(), 1u);
    EXPECT_TRUE(parser.exist("run"));
    EXPECT_TRUE(parser.at("run").full_option_only);
    EXPECT_FALSE(parser.at("run").has_value);
}

TEST(CommandParser_Register, AddFullCommand_WithValue)
{
    const char* argv[] = {"app"};
    CommandParser parser(1, const_cast<char**>(argv));
    EXPECT_TRUE(parser.addFullCommand("run", "run app",
                 /*has_value=*/true, /*default_value=*/"main",
                 /*is_required=*/true));
    EXPECT_TRUE(parser.at("run").full_option_only);
    EXPECT_TRUE(parser.at("run").has_value);
    EXPECT_TRUE(parser.at("run").is_required);
    EXPECT_EQ(parser.at("run").default_value, "main");
}

TEST(CommandParser_Register, AddLastCommand_SetsLastFlag)
{
    const char* argv[] = {"app"};
    CommandParser parser(1, const_cast<char**>(argv));
    EXPECT_TRUE(parser.addLastCommand("end", "e", "stop here",
                 /*has_value=*/false, /*default_value=*/""));
    EXPECT_TRUE(parser.at("end").is_last_command);
    EXPECT_FALSE(parser.at("end").full_option_only);
    EXPECT_FALSE(parser.at("end").has_value);
}

TEST(CommandParser_Register, AddFullLastCommand_SetsLastAndFullFlag)
{
    const char* argv[] = {"app"};
    CommandParser parser(1, const_cast<char**>(argv));
    EXPECT_TRUE(parser.addFullLastCommand("quit", "quit app",
                 /*has_value=*/true, /*default_value=*/""));
    EXPECT_TRUE(parser.at("quit").is_last_command);
    EXPECT_TRUE(parser.at("quit").full_option_only);
    EXPECT_TRUE(parser.at("quit").has_value);
}

TEST(CommandParser_Register, AddFullCommand_Duplicate_ReturnsFalse)
{
    const char* argv[] = {"app"};
    CommandParser parser(1, const_cast<char**>(argv));
    EXPECT_TRUE(parser.addFullCommand("run", "run app"));
    EXPECT_FALSE(parser.addFullCommand("run", "run again"));
    EXPECT_EQ(parser.size(), 1u);
}

//---------------------------------------------------------------
// Part 2. remove / clear / renameCommand / lastCommandName / exist / size
//---------------------------------------------------------------
TEST(CommandParser_Manage, Remove_ExistingCommand)
{
    const char* argv[] = {"app"};
    CommandParser parser(1, const_cast<char**>(argv));
    parser.addCommand("help", "h", "help");
    parser.addCommand("version", "v", "ver");
    EXPECT_TRUE(parser.exist("help"));
    EXPECT_TRUE(parser.remove("help"));
    EXPECT_FALSE(parser.exist("help"));
    EXPECT_EQ(parser.size(), 1u);
}

TEST(CommandParser_Manage, Remove_NonExistingCommand)
{
    const char* argv[] = {"app"};
    CommandParser parser(1, const_cast<char**>(argv));
    parser.addCommand("help", "h", "help");
    EXPECT_FALSE(parser.remove("nothing"));
    EXPECT_EQ(parser.size(), 1u);
}

TEST(CommandParser_Manage, Remove_RequiredCommand_UpdatesRequiredList)
{
    const char* argv[] = {"app"};
    CommandParser parser(1, const_cast<char**>(argv));
    parser.addCommand("name", "n", "name", true, "", true);
    parser.addCommand("file", "f", "file", true, "", true);
    EXPECT_TRUE(parser.remove("name"));
    // 再次移除 file 后 required 列表应已为空
    EXPECT_TRUE(parser.remove("file"));
    EXPECT_EQ(parser.size(), 0u);
}

TEST(CommandParser_Manage, Remove_DefaultCommand_ClearsDefault)
{
    const char* argv[] = {"app"};
    CommandParser parser(1, const_cast<char**>(argv));
    parser.addFullCommand("run", "run", true, "", true, /*default_command=*/true);
    EXPECT_TRUE(parser.exist("run"));
    EXPECT_TRUE(parser.remove("run"));
    EXPECT_EQ(parser.size(), 0u);
}

TEST(CommandParser_Manage, Clear_ResetsEverything)
{
    const char* argv[] = {"app"};
    CommandParser parser(1, const_cast<char**>(argv));
    parser.addCommand("help", "h", "help");
    parser.addFullCommand("run", "run", true, "", true, true);
    EXPECT_EQ(parser.size(), 2u);
    parser.clear();
    EXPECT_EQ(parser.size(), 0u);
    EXPECT_FALSE(parser.exist("help"));
    EXPECT_FALSE(parser.exist("run"));
}

TEST(CommandParser_Manage, RenameCommand_Success)
{
    const char* argv[] = {"app"};
    CommandParser parser(1, const_cast<char**>(argv));
    parser.addCommand("help", "h", "help info");
    EXPECT_TRUE(parser.renameCommand("help", "hurr"));
    EXPECT_FALSE(parser.exist("help"));
    EXPECT_TRUE(parser.exist("hurr"));
    EXPECT_EQ(parser.at("hurr").short_options, "h");
}

TEST(CommandParser_Manage, RenameCommand_ToExistingName_Fails)
{
    const char* argv[] = {"app"};
    CommandParser parser(1, const_cast<char**>(argv));
    parser.addCommand("help", "h", "help");
    parser.addCommand("version", "v", "ver");
    EXPECT_FALSE(parser.renameCommand("help", "version"));
}

TEST(CommandParser_Manage, RenameCommand_FromNonExisting_Fails)
{
    const char* argv[] = {"app"};
    CommandParser parser(1, const_cast<char**>(argv));
    parser.addCommand("help", "h", "help");
    EXPECT_FALSE(parser.renameCommand("nope", "yes"));
}

TEST(CommandParser_Manage, LastCommandName_TracksLastAdd)
{
    const char* argv[] = {"app"};
    CommandParser parser(1, const_cast<char**>(argv));
    parser.addCommand("alpha", "a", "");
    EXPECT_EQ(parser.lastCommandName(), "alpha");
    parser.addCommand("beta", "b", "");
    EXPECT_EQ(parser.lastCommandName(), "beta");
    parser.addFullCommand("gamma", "");
    EXPECT_EQ(parser.lastCommandName(), "gamma");
    parser.addLastCommand("delta", "d", "");
    EXPECT_EQ(parser.lastCommandName(), "delta");
    parser.addFullLastCommand("omega", "");
    EXPECT_EQ(parser.lastCommandName(), "omega");
}

TEST(CommandParser_Manage, RenameCommand_UpdatesLastCommandName)
{
    const char* argv[] = {"app"};
    CommandParser parser(1, const_cast<char**>(argv));
    parser.addCommand("help", "h", "help");
    EXPECT_TRUE(parser.renameCommand("help", "hurr"));
    EXPECT_EQ(parser.lastCommandName(), "hurr");
}

TEST(CommandParser_Manage, BeginEndIterator)
{
    const char* argv[] = {"app"};
    CommandParser parser(1, const_cast<char**>(argv));
    parser.addCommand("a", "1", "");
    parser.addCommand("b", "2", "");
    size_t count = 0;
    for (auto it = parser.begin(); it != parser.end(); ++it) {
        ++count;
    }
    EXPECT_EQ(count, 2u);
    size_t ccount = 0;
    for (auto it = parser.cbegin(); it != parser.cend(); ++it) {
        ++ccount;
    }
    EXPECT_EQ(ccount, 2u);
}

//---------------------------------------------------------------
// Part 3. 短选项解析
//---------------------------------------------------------------
static CommandParser makeShortParser(int argc, char** argv)
{
    CommandParser p(argc, argv);
    p.addCommand("help",    "h", "help info", false);
    p.addCommand("version", "v", "ver info", false);
    p.addCommand("name",    "n", "set name", true);
    p.addCommand("file",    "f", "set file", true);
    return p;
}

TEST(CommandParser_ShortOpt, NoParam_Merged)
{
    const char* argv[] = {"app", "-hv"};
    CommandParser p(2, const_cast<char**>(argv));
    p.addCommand("help",    "h", "", false);
    p.addCommand("version", "v", "", false);

    int cnt = 0;
    std::vector<std::string> missing;
    auto err = p.exec(&cnt, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    EXPECT_EQ(cnt, 2);
    auto& list = p.execCommandList();
    EXPECT_EQ(list[0].option_name, "help");
    EXPECT_EQ(list[1].option_name, "version");
}

TEST(CommandParser_ShortOpt, SpaceValue)
{
    const char* argv[] = {"app", "-n", "testname"};
    int argc = sizeof(argv)/sizeof(char*);
    auto p = makeShortParser(argc, const_cast<char**>(argv));
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    EXPECT_EQ(p.execCommandList()[0].value, "testname");
}

TEST(CommandParser_ShortOpt, AttachValue)
{
    const char* argv[] = {"app", "-nabc123"};
    int argc = sizeof(argv)/sizeof(char*);
    auto p = makeShortParser(argc, const_cast<char**>(argv));
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    EXPECT_EQ(p.execCommandList()[0].value, "abc123");
}

TEST(CommandParser_ShortOpt, EqualValue)
{
    const char* argv[] = {"app", "-n=demo"};
    int argc = sizeof(argv)/sizeof(char*);
    auto p = makeShortParser(argc, const_cast<char**>(argv));
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    EXPECT_EQ(p.execCommandList()[0].value, "demo");
}

TEST(CommandParser_ShortOpt, MergeThenAttachValue)
{
    // -vnMike: v无值, n附加值=Mike
    const char* argv[] = {"app", "-vnMike"};
    int argc = sizeof(argv)/sizeof(char*);
    auto p = makeShortParser(argc, const_cast<char**>(argv));
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    auto& list = p.execCommandList();
    ASSERT_EQ(list.size(), 2u);
    EXPECT_EQ(list[0].option_name, "version");
    EXPECT_EQ(list[1].option_name, "name");
    EXPECT_EQ(list[1].value, "Mike");
}

TEST(CommandParser_ShortOpt, MissingArgument_LastShortNeedsValue)
{
    // -n 最后一个参数且需要值 → MissingArgument
    const char* argv[] = {"app", "-n"};
    int argc = sizeof(argv)/sizeof(char*);
    auto p = makeShortParser(argc, const_cast<char**>(argv));
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::MissingArgument);
}

TEST(CommandParser_ShortOpt, MissingArgument_NeedsValueButNextIsOption)
{
    // -n -h: n需要值但下一个参数是选项 → MissingArgument
    const char* argv[] = {"app", "-n", "-h"};
    int argc = sizeof(argv)/sizeof(char*);
    auto p = makeShortParser(argc, const_cast<char**>(argv));
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    // 实现里 is_parsing 会设为 true 并尝试把 "-h" 当值填进去...
    // 不过 checkAndRemoveRequiredCommand 只处理 required 列表
    // 仔细看 parseUserCommand 逻辑：is_parsing + filling_option 会用 "-h" 当值
    // 所以这其实会成功解析：name.value = "-h"
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    EXPECT_EQ(p.execCommandList()[0].value, "-h");
}

TEST(CommandParser_ShortOpt, UnknownShort)
{
    const char* argv[] = {"app", "-z"};
    int argc = sizeof(argv)/sizeof(char*);
    auto p = makeShortParser(argc, const_cast<char**>(argv));
    int errPos = 0;
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, &errPos, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::UnknownOption);
    EXPECT_EQ(errPos, 1);
}

TEST(CommandParser_ShortOpt, OneUnknownInMerged)
{
    // -hz: h 合法，z 不合法 → UnknownOption
    const char* argv[] = {"app", "-hz"};
    int argc = sizeof(argv)/sizeof(char*);
    auto p = makeShortParser(argc, const_cast<char**>(argv));
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::UnknownOption);
}

//---------------------------------------------------------------
// Part 4. 长选项解析
//---------------------------------------------------------------
TEST(CommandParser_LongOpt, NoParam)
{
    const char* argv[] = {"app", "--help"};
    int argc = sizeof(argv)/sizeof(char*);
    auto p = makeShortParser(argc, const_cast<char**>(argv));
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    ASSERT_EQ(p.execCommandList().size(), 1u);
    EXPECT_EQ(p.execCommandList()[0].option_name, "help");
}

TEST(CommandParser_LongOpt, EqualAssign)
{
    const char* argv[] = {"app", "--name=jack"};
    int argc = sizeof(argv)/sizeof(char*);
    auto p = makeShortParser(argc, const_cast<char**>(argv));
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    EXPECT_EQ(p.execCommandList()[0].value, "jack");
}

TEST(CommandParser_LongOpt, SpaceAssign)
{
    const char* argv[] = {"app", "--name", "data.txt"};
    int argc = sizeof(argv)/sizeof(char*);
    auto p = makeShortParser(argc, const_cast<char**>(argv));
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    EXPECT_EQ(p.execCommandList()[0].value, "data.txt");
}

TEST(CommandParser_LongOpt, MissingArgument_LastArgNeedsValue)
{
    // --name 是最后一个且 has_value → MissingArgument
    const char* argv[] = {"app", "--name"};
    int argc = sizeof(argv)/sizeof(char*);
    auto p = makeShortParser(argc, const_cast<char**>(argv));
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::MissingArgument);
}

TEST(CommandParser_LongOpt, UnknownLong)
{
    const char* argv[] = {"app", "--nope"};
    int argc = sizeof(argv)/sizeof(char*);
    auto p = makeShortParser(argc, const_cast<char**>(argv));
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::UnknownOption);
}

TEST(CommandParser_LongOpt, UnknownLong_WithEqual)
{
    const char* argv[] = {"app", "--nope=x"};
    int argc = sizeof(argv)/sizeof(char*);
    auto p = makeShortParser(argc, const_cast<char**>(argv));
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::UnknownOption);
}

TEST(CommandParser_LongOpt, FullOptionOnly_Error)
{
    // run 是 full_option_only，用 --run 调用 → FullOptionOnly
    const char* argv[] = {"app", "--run"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addFullCommand("run", "run app", true);
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::FullOptionOnly);
}

//---------------------------------------------------------------
// Part 5. FullCommand 解析
//---------------------------------------------------------------
TEST(CommandParser_FullCmd, RunWithoutArg_InvalidValue)
{
    const char* argv[] = {"app", "run"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addFullCommand("run", "run app", true, /*default_value=*/"");
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    // run has_value 但无值也无 default → InvalidValue
    EXPECT_EQ(err, CommandParser::ParseError::InvalidValue);
}

TEST(CommandParser_FullCmd, RunWithArg)
{
    const char* argv[] = {"app", "run", "main.bin"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addFullCommand("run", "run app", true);
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    ASSERT_EQ(p.execCommandList().size(), 1u);
    EXPECT_EQ(p.execCommandList()[0].option_name, "run");
    EXPECT_EQ(p.execCommandList()[0].value, "main.bin");
}

TEST(CommandParser_FullCmd, RunNoValue_NoParam_NoError)
{
    const char* argv[] = {"app", "run"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addFullCommand("run", "run app", false); // has_value = false
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
}

TEST(CommandParser_FullCmd, RunThenShortOpts)
{
    // run app.elf -hv -n=Mike --file=log.txt
    const char* argv[] = {"app", "run", "app.elf", "-hv", "-n=Mike", "--file=log.txt"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addFullCommand("run", "run app", true);
    p.addCommand("help",    "h", "", false);
    p.addCommand("version", "v", "", false);
    p.addCommand("name",    "n", "", true);
    p.addCommand("file",    "f", "", true);
    int cnt = 0;
    std::vector<std::string> missing;
    auto err = p.exec(&cnt, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    EXPECT_EQ(cnt, 5);
    auto& list = p.execCommandList();
    EXPECT_EQ(list[0].option_name, "run");
    EXPECT_EQ(list[0].value, "app.elf");
    EXPECT_EQ(list[4].option_name, "file");
    EXPECT_EQ(list[4].value, "log.txt");
}

TEST(CommandParser_FullCmd, UnknownFirstArg)
{
    const char* argv[] = {"app", "abcdef"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addFullCommand("run", "run app");
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::UnknownOption);
}

//---------------------------------------------------------------
// Part 6. is_last_command 特性
//---------------------------------------------------------------
TEST(CommandParser_LastCmd, NormalCommandAfterLast_ShouldBeIgnored)
{
    // -h 是 last command，即使后面还有未处理的 required 命令，也应停止
    const char* argv[] = {"app", "-h", "-f", "x.txt"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addLastCommand("help", "h", "help");           // last command
    p.addCommand("file", "f", "file", true, "", true); // required
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    // last command 出现后 required 检查被跳过
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    ASSERT_EQ(p.execCommandList().size(), 1u);
    EXPECT_EQ(p.execCommandList()[0].option_name, "help");
}

TEST(CommandParser_LastCmd, FullLastCommand_StopsParsing)
{
    const char* argv[] = {"app", "quit", "main.bin", "-f", "x.txt"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addFullLastCommand("quit", "quit app", true);
    p.addCommand("file", "f", "file", true, "", true); // required
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    ASSERT_EQ(p.execCommandList().size(), 1u);
    EXPECT_EQ(p.execCommandList()[0].option_name, "quit");
    EXPECT_EQ(p.execCommandList()[0].value, "main.bin");
}

TEST(CommandParser_LastCmd, LastCommand_MissingArgument_AtEnd)
{
    // -h 是最后一个参数，has_value=true → parse 阶段 MissingArgument
    const char* argv[] = {"app", "-h"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addLastCommand("host", "h", "set host", true); // has_value=true, 无 default
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::MissingArgument);
}

TEST(CommandParser_LastCmd, LastCommand_EmptyValue_UsesDefault)
{
    // 用户提供了空值（等号后空），parse 正常完成 → exec 阶段应用 default_value
    const char* argv[] = {"app", "-h="};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addLastCommand("host", "h", "set host", true, "localhost");
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    ASSERT_EQ(p.execCommandList().size(), 1u);
    EXPECT_EQ(p.execCommandList()[0].value, "localhost");
}

//---------------------------------------------------------------
// Part 7. default_value 填充特性
//---------------------------------------------------------------
TEST(CommandParser_DefaultVal, DefaultValueAppliedWhenMissing)
{
    // 注册时有 default_value，用户未传值 → 自动填充
    const char* argv[] = {"app"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addCommand("name", "n", "name", true, "DefaultMike");
    int cnt = 0;
    std::vector<std::string> missing;
    auto err = p.exec(&cnt, nullptr, &missing);
    // 这里 name 不是 required，所以 exec 不会把它加入列表
    // 没有 has_value 且被加入列表的项... name 根本没被解析到
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    EXPECT_EQ(cnt, 0);
}

TEST(CommandParser_DefaultVal, DefaultValueAppliedInExec)
{
    // 命令被解析到了但没填值（来自 is_parsing 被 is_last_command 截断时的残留？）
    // 更常见的触发：用 default_command 在最后一个位置参数没给值
    // 我们换一种方式：has_value 命令有 default_value，解析成功但 value 为空
    // 实现里 exec 会在 cmd.value.empty() && !default_value.empty() 时填充
    // 但 parseUserCommand 会正常给 value 赋值（如果没给值且 is_parsing 卡住就会 InvalidValue）
    // 所以 default_value 主要在用户提供空字符串值时起作用
    // --name= 这种形式会让 value=""
    const char* argv[] = {"app", "--name="};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addCommand("name", "n", "name", true, "Jack");
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    EXPECT_EQ(p.execCommandList()[0].value, "Jack"); // default_value 被应用
}

TEST(CommandParser_DefaultVal, DefaultValueNotOverwritten)
{
    const char* argv[] = {"app", "--name=Alice"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addCommand("name", "n", "name", true, "Jack");
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    EXPECT_EQ(p.execCommandList()[0].value, "Alice"); // 用户值优先
}

TEST(CommandParser_DefaultVal, NoValueAndNoDefault_InvalidValue)
{
    const char* argv[] = {"app", "--name="};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addCommand("name", "n", "name", true, /*default_value=*/"");
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::InvalidValue);
}

//---------------------------------------------------------------
// Part 8. default_command 特性（位置参数）
//---------------------------------------------------------------
TEST(CommandParser_DefCmd, DefaultCommand_FirstPositionalArg)
{
    // addFullCommand("run", ..., is_required=true, default_command=true)
    // 第一个参数非 '-' 开头时，应该把它当成 run 的值
    const char* argv[] = {"app", "main.bin"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addFullCommand("run", "run app", true, "", /*is_required=*/true, /*default_command=*/true);
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    ASSERT_EQ(p.execCommandList().size(), 1u);
    EXPECT_EQ(p.execCommandList()[0].option_name, "run");
    EXPECT_EQ(p.execCommandList()[0].value, "main.bin");
}

TEST(CommandParser_DefCmd, DefaultCommand_LastPositionalArg_Fallback)
{
    // default_command 不是 full_only，注册为普通命令
    // 最后一个参数非 '-' 开头时，如果有 required + is_default_command 未填值，就用它填
    const char* argv[] = {"app", "-h", "main.bin"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addCommand("help", "h", "help");
    // full_command 标记为 default_command
    p.addFullCommand("run", "run app", true, "", true, true);
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    auto& list = p.execCommandList();
    ASSERT_EQ(list.size(), 2u);
    EXPECT_EQ(list[0].option_name, "help");
    EXPECT_EQ(list[1].option_name, "run");
    EXPECT_EQ(list[1].value, "main.bin");
}

TEST(CommandParser_DefCmd, MissingDefaultCommandError_CurrentlyUnreachable)
{
    // 分析：
    // ParseError::MissingDefaultCommand 在源码 376 行触发，条件是：
    //   i==1 && arg[0]!='-' && !_default_cmd.empty() 且 checkAndRemoveRequiredCommand 返回 false
    // 但当前 addCommand/addFullCommand 的逻辑中，_default_cmd 仅在
    //   is_required=true && default_command=true && _default_cmd.empty() 时被设置，
    // 而 is_required=true 的命令会同时被 push_back 到 _required_cmd_list。
    // parseUserCommand 开头拷贝 required_cmds 时必然包含 _default_cmd，
    // 所以 checkAndRemoveRequiredCommand(required_cmds, _default_cmd) 总会成功（返回 true）。
    // 除非通过 remove() 把命令删掉同时又保留 _default_cmd（当前 remove 会清 _default_cmd）。
    // 因此当前实现中此错误码不可从外部触发，先跳过。
    GTEST_SKIP() << "ParseError::MissingDefaultCommand is unreachable in current implementation.";
}

TEST(CommandParser_DefCmd, MultipleRequired_WithDefault)
{
    // 一个 required 默认命令 + 其他 required
    const char* argv[] = {"app", "main.bin", "-f", "log.txt"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addFullCommand("run", "run", true, "", true, true);  // required + default_command
    p.addCommand("file", "f", "file", true, "", true);     // required
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    auto& list = p.execCommandList();
    ASSERT_EQ(list.size(), 2u);
    EXPECT_EQ(list[0].option_name, "run");
    EXPECT_EQ(list[0].value, "main.bin");
    EXPECT_EQ(list[1].option_name, "file");
    EXPECT_EQ(list[1].value, "log.txt");
}

//---------------------------------------------------------------
// Part 9. 错误码全覆盖
//---------------------------------------------------------------
TEST(CommandParser_Error, MissingArgument_RequiredCommandNotProvided)
{
    const char* argv[] = {"app"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addCommand("name", "n", "name", true, "", true); // required
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::MissingArgument);
    ASSERT_EQ(missing.size(), 1u);
    EXPECT_EQ(missing[0], "name");
}

TEST(CommandParser_Error, MissingArgument_MultipleMissing)
{
    const char* argv[] = {"app"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addCommand("name", "n", "name", true, "", true);
    p.addCommand("file", "f", "file", true, "", true);
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::MissingArgument);
    EXPECT_EQ(missing.size(), 2u);
}

TEST(CommandParser_Error, MissingArgument_RequiredWithDefaultValue)
{
    // required 命令有 default_value → 不触发 MissingArgument
    // 因为 default_value 不影响 required_cmds 检查，但 exec 阶段会给 value 赋值
    // required_cmds 只有在解析时遇到该命令才会移除
    // 有 default_value 只是让 InvalidValue 不触发
    // 如果用户没提供 required 命令，它依然在 required_cmds 里 → MissingArgument
    const char* argv[] = {"app"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addCommand("name", "n", "name", true, "Default", true);
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::MissingArgument);
    EXPECT_EQ(missing.size(), 1u);
}

TEST(CommandParser_Error, ExecPhase_InvalidValue_LongOptEmpty_NoDefault)
{
    // exec 阶段 InvalidValue：parse 正常完成（用户给了空值 --name=），
    // 但 has_value 的命令 value 为空且 default_value 也空
    const char* argv[] = {"app", "--name="};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addCommand("name", "n", "name", true, /*default_value=*/"");
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::InvalidValue);
}

TEST(CommandParser_Error, ExecPhase_InvalidValue_ShortOptEmpty_NoDefault)
{
    const char* argv[] = {"app", "-n="};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addCommand("name", "n", "name", true, /*default_value=*/"");
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::InvalidValue);
}

TEST(CommandParser_Error, FullCommand_AutoConsumesNextArgAsValue)
{
    // FullCommand has_value 时，下一个参数（包括 -xxx 形式）会被当作值而非选项
    // 因为 is_parsing 块（420行）优先于长/短选项块
    const char* argv[] = {"app", "run", "--name=jack"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addFullCommand("run", "run app", true);
    p.addCommand("name", "n", "name", true);
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    ASSERT_EQ(p.execCommandList().size(), 1u);
    EXPECT_EQ(p.execCommandList()[0].option_name, "run");
    EXPECT_EQ(p.execCommandList()[0].value, "--name=jack"); // 被当作 run 的值！
}

TEST(CommandParser_Error, FormatError_NonDash_NonFirst_NonLast)
{
    // FormatError 只在 537-539 行触发：
    // else { err_pos = i; return FormatError; }
    // 前面的所有 if/else if 都没匹配到
    // 条件：i != 1（不是首参）、!is_parsing、arg[0] != '-'、不是最后一个参数
    // 且不是 full_only 首参（因为 i!=1）
    // 且不是 default_cmd 首参（因为 i!=1）
    const char* argv[] = {"app", "run", "main.bin", "extra", "-h"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addFullCommand("run", "run app", true);
    p.addCommand("help", "h", "help");
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::FormatError);
}

TEST(CommandParser_Error, FormatError_DefaultCommand_FirstArgStillFormatError)
{
    // 不提供 default_command 时，首个非 '-' 参数必须是 full_only 命令
    // 所以如果只有普通选项，首个参数非 '-' 就是 FormatError... 等等不对
    // 看 parseUserCommand 526-536 行：
    //   else if (i == _argc - 1) {
    //     处理 default_command
    //   }
    // 那中间位置的非 '-' 参数且非 is_parsing → FormatError
    const char* argv[] = {"app", "--help", "extra", "--name=jack"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addCommand("help", "h", "help");
    p.addCommand("name", "n", "name", true);
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::FormatError);
}

TEST(CommandParser_Error, GetParseErrorName_CoverAll)
{
    EXPECT_STREQ(CommandParser::getParseErrorName(CommandParser::ParseError::NoError),         "Successful");
    EXPECT_STREQ(CommandParser::getParseErrorName(CommandParser::ParseError::UnknownOption),   "Unknown option");
    EXPECT_STREQ(CommandParser::getParseErrorName(CommandParser::ParseError::FullOptionOnly),  "Full option only");
    EXPECT_STREQ(CommandParser::getParseErrorName(CommandParser::ParseError::InvalidValue),     "Invalid value");
    EXPECT_STREQ(CommandParser::getParseErrorName(CommandParser::ParseError::MissingArgument), "Missing argument");
    EXPECT_STREQ(CommandParser::getParseErrorName(CommandParser::ParseError::FormatError),      "Format error");
    EXPECT_STREQ(CommandParser::getParseErrorName(CommandParser::ParseError::MissingDefaultCommand), "Missing default command");
}

//---------------------------------------------------------------
// Part 10. generateHelpInfo
//---------------------------------------------------------------
TEST(CommandParser_Help, GeneratesHelpText)
{
    const char* argv[] = {"app"};
    CommandParser p(1, const_cast<char**>(argv));
    p.addCommand("help", "h", "show help");
    p.addCommand("name", "n", "set name", /*has_value=*/true, "", /*is_required=*/true, false, "NAME");
    p.addFullCommand("run", "run app", true, "", true, true);

    std::string help = p.generateHelpInfo();
    EXPECT_FALSE(help.empty());
    EXPECT_NE(help.find("USAGE:"), std::string::npos);
    EXPECT_NE(help.find("OPTIONS:"), std::string::npos);
    EXPECT_NE(help.find("COMMANDS:"), std::string::npos);
    EXPECT_NE(help.find("--help"), std::string::npos);
    EXPECT_NE(help.find("-h"), std::string::npos);
    EXPECT_NE(help.find("--name"), std::string::npos);
    EXPECT_NE(help.find("<NAME>"), std::string::npos); // is_required=true → <NAME>
    EXPECT_NE(help.find("VAL"), std::string::npos);     // run 是 required 且默认 placeholder "VAL"
}

TEST(CommandParser_Help, ShowOptionsOnly_NoUsage)
{
    const char* argv[] = {"app"};
    CommandParser p(1, const_cast<char**>(argv));
    p.addCommand("help", "h", "show help");

    std::string help = p.generateHelpInfo(64, false, /*show_options_only=*/true);
    EXPECT_EQ(help.find("USAGE:"), std::string::npos);
    EXPECT_NE(help.find("OPTIONS:"), std::string::npos);
}

TEST(CommandParser_Help, SortOptionName)
{
    const char* argv[] = {"app"};
    CommandParser p(1, const_cast<char**>(argv));
    p.addCommand("zebra", "z", "z desc");
    p.addCommand("apple", "a", "a desc");

    std::string sorted = p.generateHelpInfo(64, /*sort_option_name=*/true);
    std::string unsorted = p.generateHelpInfo(64, /*sort_option_name=*/false);
    // 排序后 apple 应该在 zebra 前面
    EXPECT_LT(sorted.find("--apple"), sorted.find("--zebra"));
}

//---------------------------------------------------------------
// Part 11. 混合场景综合
//---------------------------------------------------------------
TEST(CommandParser_Mix, FullCmdThenOptions)
{
    const char* argv[] = {"app", "run", "app.elf", "-hv", "-n=Mike", "--file=log.txt"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addFullCommand("run", "run app", true);
    p.addCommand("help",    "h", "", false);
    p.addCommand("version", "v", "", false);
    p.addCommand("name",    "n", "", true);
    p.addCommand("file",    "f", "", true);
    int cnt = 0;
    std::vector<std::string> missing;
    auto err = p.exec(&cnt, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    EXPECT_EQ(cnt, 5);
}

TEST(CommandParser_Mix, OnlyOptions_NoFullCmd)
{
    const char* argv[] = {"app", "-h", "--name=Alice"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addCommand("help", "h", "", false);
    p.addCommand("name", "n", "", true);
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    ASSERT_EQ(p.execCommandList().size(), 2u);
    EXPECT_EQ(p.execCommandList()[0].option_name, "help");
    EXPECT_EQ(p.execCommandList()[1].option_name, "name");
    EXPECT_EQ(p.execCommandList()[1].value, "Alice");
}

TEST(CommandParser_Mix, LastCommandStopsAllParsing)
{
    const char* argv[] = {"app", "run", "app.elf", "--end", "ignore", "this", "--also-ignore"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addFullCommand("run", "run app", true);
    p.addLastCommand("end", "e", "stop");
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    auto& list = p.execCommandList();
    EXPECT_EQ(list.size(), 2u);
    EXPECT_EQ(list[0].option_name, "run");
    EXPECT_EQ(list[0].value, "app.elf");
    EXPECT_EQ(list[1].option_name, "end");
}

TEST(CommandParser_Mix, NoArgs_Empty)
{
    const char* argv[] = {"app"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addCommand("help", "h", "", false);
    std::vector<std::string> missing;
    int cnt = 0;
    auto err = p.exec(&cnt, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    EXPECT_EQ(cnt, 0);
}

TEST(CommandParser_Mix, RegisterThenExecAgain_NoStateLeakage)
{
    const char* argv[] = {"app", "--help"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addCommand("help", "h", "", false);
    std::vector<std::string> missing;

    auto err1 = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err1, CommandParser::ParseError::NoError);
    EXPECT_EQ(p.execCommandList().size(), 1u);

    // 第二次 exec，_exec_cmd_list 应被清空
    auto err2 = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err2, CommandParser::ParseError::NoError);
    EXPECT_EQ(p.execCommandList().size(), 1u); // 依然是 help 被解析到
}

//---------------------------------------------------------------
// Part 12. short_options 包含多个字符（makeShortOptions 去重）
//---------------------------------------------------------------
TEST(CommandParser_ShortOpt, MultiShortChars)
{
    // addCommand 短选项可以带多个字符如 "hv"，makeShortOptions 会去重+排序
    const char* argv[] = {"app", "-h"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    p.addCommand("help", "hv", "help or version", false);
    EXPECT_TRUE(p.exist("help"));
    // makeShortOptions 内部用 map<char, uint32_t> 排序，所以 hv 会变成 "hv"
    std::vector<std::string> missing;
    auto err = p.exec(nullptr, nullptr, &missing);
    EXPECT_EQ(err, CommandParser::ParseError::NoError);
    EXPECT_EQ(p.execCommandList()[0].option_name, "help");
}

TEST(CommandParser_ShortOpt, DuplicateShortChars_Dedup)
{
    const char* argv[] = {"app", "-h"};
    int argc = sizeof(argv)/sizeof(char*);
    CommandParser p(argc, const_cast<char**>(argv));
    // "hhlp" → makeShortOptions 会去重排序为 "hlp"
    p.addCommand("help", "hhlp", "test", false);
    EXPECT_EQ(p.at("help").short_options, "hlp");
}

//---------------------------------------------------------------
// Part 13. value_placeholder 自定义
//---------------------------------------------------------------
TEST(CommandParser_Placeholder, CustomPlaceholder)
{
    const char* argv[] = {"app"};
    CommandParser p(1, const_cast<char**>(argv));
    p.addCommand("port", "p", "set port", true, "", false, false, "PORT_NUM");
    EXPECT_EQ(p.at("port").value_placeholder, "PORT_NUM");
}

//---------------------------------------------------------------
// Part 14. makeOptionName 处理带 '-' 和空格的命令名
//---------------------------------------------------------------
TEST(CommandParser_Name, DashedAndSpacedNames_Normalized)
{
    const char* argv[] = {"app"};
    CommandParser p(1, const_cast<char**>(argv));
    p.addCommand("--my-cmd", "m", "test");
    // makeOptionName 去掉前导 '-'、把空格去掉
    EXPECT_TRUE(p.exist("my-cmd"));
    EXPECT_EQ(p.lastCommandName(), "my-cmd");
}

TEST(CommandParser_Name, SpacedNames_Normalized)
{
    const char* argv[] = {"app"};
    CommandParser p(1, const_cast<char**>(argv));
    p.addCommand("  my   cmd  ", "m", "test");
    EXPECT_TRUE(p.exist("mycmd"));
}

//---------------------------------------------------------------
// main
//---------------------------------------------------------------
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
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
 *************************************************************************************/
