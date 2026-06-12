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
#include "../src/TUI/Terminal.hpp"

using namespace Tiny;

// 无参数选项测试专用
static CommandParser createNoParamParser(int argc, char** argv)
{
    CommandParser parser(argc, argv);
    parser.addCommand("help",    "h", "help info", false);
    parser.addCommand("version", "v", "ver info", false);
    return parser;
}

// 需要参数的选项测试专用
static CommandParser createNeedParamParser(int argc, char** argv)
{
    CommandParser parser(argc, argv);
    parser.addCommand("name", "n", "set name", true, "", true);
    parser.addCommand("file", "f", "set file", true, "", false);
    return parser;
}

// Full command 测试专用
static CommandParser createFullCommandParser(int argc, char** argv)
{
    CommandParser parser(argc, argv);
    parser.addFullCommand("run", "run app", true, "", true);
    return parser;
}

// 混合命令测试专用
static CommandParser createMixedParser(int argc, char** argv)
{
    CommandParser parser(argc, argv);
    parser.addCommand("help",    "h", "help info", false);
    parser.addCommand("version", "v", "ver info", false);
    parser.addCommand("name",    "n", "set name", true, "", true);
    parser.addCommand("file",    "f", "set file", true, "", true);
    parser.addFullCommand("run", "run app", true, "", true);
    return parser;
}

// 错误测试专用（包含所有类型命令）
static CommandParser createErrorTestParser(int argc, char** argv)
{
    CommandParser parser(argc, argv);
    parser.addCommand("help",    "h", "help info", false);
    parser.addCommand("version", "v", "ver info", false);
    parser.addCommand("name",    "n", "set name", true, "", true);
    parser.addCommand("file",    "f", "set file", true, "", true);
    parser.addFullCommand("run", "run app", true, "", true);
    return parser;
}

//=====================短选项测试=====================
TEST(CommandParserTest, ShortOpt_NoParam)
{
    const char* argv[] = {"app", "-hv"};
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createNoParamParser(argc, const_cast<char**>(argv));

    int cnt = 0, errPos = 0;
    std::vector<std::string> missingList;
    auto ret = parser.exec(&cnt, &errPos, &missingList);
    EXPECT_EQ(ret, CommandParser::ParseError::NoError);
    EXPECT_EQ(cnt, 2);

    auto& list = parser.execCommandList();
    EXPECT_EQ(list[0].option_name, "help");
    EXPECT_EQ(list[1].option_name, "version");
}

TEST(CommandParserTest, ShortOpt_SpaceValue)
{
    const char* argv[] = {"app", "-n", "testname"};
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createNeedParamParser(argc, const_cast<char**>(argv));

    int cnt = 0;
    std::vector<std::string> missingList;
    auto ret = parser.exec(&cnt, nullptr, &missingList);
    EXPECT_EQ(cnt, 1);
    EXPECT_EQ(ret, CommandParser::ParseError::NoError);
    EXPECT_EQ(parser.execCommandList()[0].value, "testname");
}

TEST(CommandParserTest, ShortOpt_AttachValue)
{
    const char* argv[] = {"app", "-nabc123"};
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createNeedParamParser(argc, const_cast<char**>(argv));
    std::vector<std::string> missingList;
    auto ret = parser.exec(nullptr,nullptr,&missingList);
    EXPECT_EQ(ret, CommandParser::ParseError::NoError);
    EXPECT_EQ(parser.execCommandList()[0].value, "abc123");
}

TEST(CommandParserTest, ShortOpt_EqualValue)
{
    const char* argv[] = {"app", "-n=demo"};
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createNeedParamParser(argc, const_cast<char**>(argv));
    std::vector<std::string> missingList;
    auto ret = parser.exec(nullptr,nullptr,&missingList);
    EXPECT_EQ(parser.execCommandList()[0].value, "demo");
}

//=====================长选项测试=====================
TEST(CommandParserTest, LongOpt_NoParam)
{
    const char* argv[] = {"app", "--help"};
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createNoParamParser(argc, const_cast<char**>(argv));
    std::vector<std::string> missingList;
    auto ret = parser.exec(nullptr,nullptr,&missingList);
    EXPECT_EQ(ret, CommandParser::ParseError::NoError);
}

TEST(CommandParserTest, LongOpt_EqualAssign)
{
    const char* argv[] = {"app", "--name=jack"};
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createNeedParamParser(argc, const_cast<char**>(argv));
    std::vector<std::string> missingList;
    auto ret = parser.exec(nullptr,nullptr,&missingList);
    EXPECT_EQ(parser.execCommandList()[0].value, "jack");
}

TEST(CommandParserTest, LongOpt_SpaceAssign)
{
    const char* argv[] = {"app", "--name", "data.txt"};
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createNeedParamParser(argc, const_cast<char**>(argv));
    std::vector<std::string> missingList;
    auto ret = parser.exec(nullptr,nullptr,&missingList);
    EXPECT_EQ(parser.execCommandList()[0].value, "data.txt");
}

//=====================全局主命令（full command）=====================
TEST(CommandParserTest, FullCommand_Only)
{
    const char* argv[] = {"app", "run"};
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createFullCommandParser(argc, const_cast<char**>(argv));
    int errpos;
    std::vector<std::string> missingList;
    auto ret = parser.exec(nullptr,&errpos,&missingList);
    // run需要参数，无值会使用默认值(空)触发InvalidValue
    EXPECT_EQ(ret, CommandParser::ParseError::InvalidValue);
}

TEST(CommandParserTest, FullCommand_WithArg)
{
    const char* argv[] = {"app", "run", "main.bin"};
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createFullCommandParser(argc, const_cast<char**>(argv));
    std::vector<std::string> missingList;
    auto ret = parser.exec(nullptr,nullptr,&missingList);
    EXPECT_EQ(ret, CommandParser::ParseError::NoError);
    EXPECT_EQ(parser.execCommandList()[0].value, "main.bin");
}

//=====================异常错误码全覆盖=====================
TEST(CommandParserTest, Err_UnknownOption)
{
    const char* argv[] = {"app", "-z"}; // z未注册
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createErrorTestParser(argc, const_cast<char**>(argv));
    int errPos;
    std::vector<std::string> missingList;
    auto ret = parser.exec(nullptr, &errPos, &missingList);
    EXPECT_EQ(ret, CommandParser::ParseError::UnknownOption);
}

TEST(CommandParserTest, Err_MissingArgument)
{
    const char* argv[] = {"app", "-n"}; // n需要参数但是缺失
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createErrorTestParser(argc, const_cast<char**>(argv));
    std::vector<std::string> missingList;
    auto ret = parser.exec(nullptr,nullptr,&missingList);
    EXPECT_EQ(ret, CommandParser::ParseError::MissingArgument);
}

TEST(CommandParserTest, Err_FullOptAsLongOpt)
{
    const char* argv[] = {"app", "--run"}; // run是full_only，不能用--
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createErrorTestParser(argc, const_cast<char**>(argv));
    std::vector<std::string> missingList;
    auto ret = parser.exec(nullptr,nullptr,&missingList);
    EXPECT_EQ(ret, CommandParser::ParseError::FullOptionOnly);
}

TEST(CommandParserTest, Err_FormatError)
{
    const char* argv[] = {"app", "abcdef"}; // 首参数非指令
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createErrorTestParser(argc, const_cast<char**>(argv));
    std::vector<std::string> missingList;
    auto ret = parser.exec(nullptr,nullptr,&missingList);
    EXPECT_EQ(ret, CommandParser::ParseError::UnknownOption);
}

//=====================混合参数场景=====================
TEST(CommandParserTest, Mix_AllParam)
{
    const char* argv[] = {"app", "run", "app.elf", "-hv", "-n=Mike", "--file=log.txt"};
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createMixedParser(argc, const_cast<char**>(argv));
    int count =0;
    std::vector<std::string> missingList;
    auto ret = parser.exec(&count,nullptr,&missingList);
    EXPECT_EQ(ret, CommandParser::ParseError::NoError);
    EXPECT_EQ(count,5);
}

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
 *                                                                                   *
 *************************************************************************************/