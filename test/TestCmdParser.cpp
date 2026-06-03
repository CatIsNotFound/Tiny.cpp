#include <gtest/gtest.h>
#include "../src/Parser/CommandParser.hpp"
#include "../src/TUI/Terminal.hpp"

using namespace Tiny;

// 封装快捷构造
static CommandParser createParser(int argc, char** argv)
{
    CommandParser parser(argc, argv);
    // 预置固定测试指令
    // 短选项: h/v无参; n/f需要参数
    parser.addCommand("help",    "h", "help info", false);
    parser.addCommand("version", "v", "ver info", false);
    parser.addCommand("name",    "n", "set name", true);
    parser.addCommand("file",    "f", "set file", true);
    // 纯主命令（不带-开头）
    parser.addFullCommand("run", "run app", true);
    return parser;
}

//=====================短选项测试=====================
TEST(CommandParserTest, ShortOpt_NoParam)
{
    const char* argv[] = {"app", "-hv"};
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createParser(argc, const_cast<char**>(argv));

    int cnt = 0, errPos = 0;
    auto ret = parser.exec(&cnt, &errPos, nullptr);
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
    auto parser = createParser(argc, const_cast<char**>(argv));

    int cnt = 0;
    auto ret = parser.exec(&cnt, nullptr, nullptr);
    EXPECT_EQ(cnt, 1);
    EXPECT_EQ(ret, CommandParser::ParseError::NoError);
    EXPECT_EQ(parser.execCommandList()[0].value, "testname");
}

TEST(CommandParserTest, ShortOpt_AttachValue)
{
    const char* argv[] = {"app", "-nabc123"};
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createParser(argc, const_cast<char**>(argv));
    auto ret = parser.exec(nullptr,nullptr,nullptr);
    EXPECT_EQ(ret, CommandParser::ParseError::NoError);
    EXPECT_EQ(parser.execCommandList()[0].value, "abc123");
}

TEST(CommandParserTest, ShortOpt_EqualValue)
{
    const char* argv[] = {"app", "-n=demo"};
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createParser(argc, const_cast<char**>(argv));
    auto ret = parser.exec(nullptr,nullptr,nullptr);
    EXPECT_EQ(parser.execCommandList()[0].value, "demo");
}

//=====================长选项测试=====================
TEST(CommandParserTest, LongOpt_NoParam)
{
    const char* argv[] = {"app", "--help"};
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createParser(argc, const_cast<char**>(argv));
    auto ret = parser.exec(nullptr,nullptr,nullptr);
    EXPECT_EQ(ret, CommandParser::ParseError::NoError);
}

TEST(CommandParserTest, LongOpt_EqualAssign)
{
    const char* argv[] = {"app", "--name=jack"};
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createParser(argc, const_cast<char**>(argv));
    auto ret = parser.exec(nullptr,nullptr,nullptr);
    EXPECT_EQ(parser.execCommandList()[0].value, "jack");
}

TEST(CommandParserTest, LongOpt_SpaceAssign)
{
    const char* argv[] = {"app", "--name", "data.txt"};
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createParser(argc, const_cast<char**>(argv));
    auto ret = parser.exec(nullptr,nullptr,nullptr);
    EXPECT_EQ(parser.execCommandList()[0].value, "data.txt");
}

//=====================全局主命令（full command）=====================
TEST(CommandParserTest, FullCommand_Only)
{
    const char* argv[] = {"app", "run"};
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createParser(argc, const_cast<char**>(argv));
    int errpos;
    auto ret = parser.exec(nullptr,&errpos,nullptr);
    // run需要参数，无值会使用默认值(空)触发InvalidValue
    EXPECT_EQ(ret, CommandParser::ParseError::InvalidValue);
}

TEST(CommandParserTest, FullCommand_WithArg)
{
    const char* argv[] = {"app", "run", "main.bin"};
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createParser(argc, const_cast<char**>(argv));
    auto ret = parser.exec(nullptr,nullptr,nullptr);
    EXPECT_EQ(ret, CommandParser::ParseError::NoError);
    EXPECT_EQ(parser.execCommandList()[0].value, "main.bin");
}

//=====================异常错误码全覆盖=====================
TEST(CommandParserTest, Err_UnknownOption)
{
    const char* argv[] = {"app", "-z"}; // z未注册
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createParser(argc, const_cast<char**>(argv));
    int errPos;
    auto ret = parser.exec(nullptr, &errPos, nullptr);
    EXPECT_EQ(ret, CommandParser::ParseError::UnknownOption);
}

TEST(CommandParserTest, Err_MissingArgument)
{
    const char* argv[] = {"app", "-n"}; // n需要参数但是缺失
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createParser(argc, const_cast<char**>(argv));
    auto ret = parser.exec(nullptr,nullptr,nullptr);
    EXPECT_EQ(ret, CommandParser::ParseError::MissingArgument);
}

TEST(CommandParserTest, Err_FullOptAsLongOpt)
{
    const char* argv[] = {"app", "--run"}; // run是full_only，不能用--
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createParser(argc, const_cast<char**>(argv));
    auto ret = parser.exec(nullptr,nullptr,nullptr);
    EXPECT_EQ(ret, CommandParser::ParseError::FullOptionOnly);
}

TEST(CommandParserTest, Err_FormatError)
{
    const char* argv[] = {"app", "abcdef"}; // 首参数非指令
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createParser(argc, const_cast<char**>(argv));
    auto ret = parser.exec(nullptr,nullptr,nullptr);
    EXPECT_EQ(ret, CommandParser::ParseError::UnknownOption);
}

//=====================混合参数场景=====================
TEST(CommandParserTest, Mix_AllParam)
{
    const char* argv[] = {"app", "run", "app.elf", "-hv", "-n=Mike", "--file=log.txt"};
    int argc = sizeof(argv)/sizeof(char*);
    auto parser = createParser(argc, const_cast<char**>(argv));
    int count =0;
    auto ret = parser.exec(&count,nullptr,nullptr);
    EXPECT_EQ(ret, CommandParser::ParseError::NoError);
    EXPECT_EQ(count,5);
}

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}