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

#include <tuple>
#include <string>

#include "../src/TUI/Terminal.hpp"   // 需包含 class Terminal 的声明

using Tiny::TUI::Terminal;

// ============================================================================
// 1. 基本替换与转义
// ============================================================================
TEST(FormatBasic, SingleSubstitution) {
    EXPECT_EQ(Terminal::formatString("{}", 42), "42");
    EXPECT_EQ(Terminal::formatString("{}", "hi"), "hi");
    EXPECT_EQ(Terminal::formatString("{}", 3.14), "3.14");
    EXPECT_EQ(Terminal::formatString("{}", true), "1");     // NOTE: ostream 默认输出 1
    EXPECT_EQ(Terminal::formatString("{}", false), "0");
    EXPECT_EQ(Terminal::formatString("{}", 'A'), "A");
}

TEST(FormatBasic, MultiSubstitution) {
    EXPECT_EQ(Terminal::formatString("{} + {} = {}", 1, 2, 3), "1 + 2 = 3");
    EXPECT_EQ(Terminal::formatString("[{}|{}|{}]", "a", 2, 3.0), "[a|2|3]");
}

TEST(FormatBasic, EscapeBraces) {
    EXPECT_EQ(Terminal::formatString("{{}}"), "{}");
    EXPECT_EQ(Terminal::formatString("a{{b}}c"), "a{b}c");
    EXPECT_EQ(Terminal::formatString("{{{{}}}}"), "{{}}");
    EXPECT_EQ(Terminal::formatString("{}"), "");   // 无参数、无内容
}

TEST(FormatBasic, EmptyString) {
    EXPECT_EQ(Terminal::formatString(""), "");
    EXPECT_EQ(Terminal::formatString("plain text"), "plain text");
}

// ============================================================================
// 2. 字符串 / 宽度对齐（s 说明符）—— 参数化
// ============================================================================
class StringAlignIntTest : public ::testing::TestWithParam<
    std::tuple<const char*, int, std::string>> {};

TEST_P(StringAlignIntTest, Align) {
    EXPECT_EQ(Terminal::formatString(std::get<0>(GetParam()),
                                     std::get<1>(GetParam())),
              std::get<2>(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(
    Ints, StringAlignIntTest,
    ::testing::Values(
        std::make_tuple("{:s}",   42, std::string("42")),
        std::make_tuple("{:5s}",  42, std::string("   42")),
        std::make_tuple("{:<5s}", 42, std::string("42   ")),
        std::make_tuple("{:>5s}", 42, std::string("   42")),
        std::make_tuple("{:5s}",  -42, std::string("  -42")),
        std::make_tuple("{:1s}",  42, std::string("42"))   // width < len：不截断
    ));

class StringAlignStrTest : public ::testing::TestWithParam<
    std::tuple<const char*, std::string, std::string>> {};

TEST_P(StringAlignStrTest, Align) {
    EXPECT_EQ(Terminal::formatString(std::get<0>(GetParam()),
                                     std::get<1>(GetParam())),
              std::get<2>(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(
    Strs, StringAlignStrTest,
    ::testing::Values(
        std::make_tuple("{:s}",   std::string("abc"), std::string("abc")),
        std::make_tuple("{:5s}",  std::string("abc"), std::string("  abc")),
        std::make_tuple("{:<5s}", std::string("abc"), std::string("abc  ")),
        std::make_tuple("{:>5s}", std::string("abc"), std::string("  abc"))
    ));

TEST(FormatString, BoolAndChar) {
    EXPECT_EQ(Terminal::formatString("{:s}",  true),  "true");
    EXPECT_EQ(Terminal::formatString("{:s}",  false), "false");
    EXPECT_EQ(Terminal::formatString("{:<6s}", false), "false ");
    EXPECT_EQ(Terminal::formatString("{:6s}",  true),  "  true");
    EXPECT_EQ(Terminal::formatString("{:3s}",  'A'),   "  A");
    EXPECT_EQ(Terminal::formatString("{:<3s}", 'A'),   "A  ");
}

// ============================================================================
// 3. 定点小数（f）
// ============================================================================
class FixedFloatTest : public ::testing::TestWithParam<
    std::tuple<const char*, double, std::string>> {};

TEST_P(FixedFloatTest, Fixed) {
    EXPECT_EQ(Terminal::formatString(std::get<0>(GetParam()),
                                     std::get<1>(GetParam())),
              std::get<2>(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(
    Floats, FixedFloatTest,
    ::testing::Values(
        std::make_tuple("{:f}",      3.14159, std::string("3.141590")),  // 默认 6 位
        std::make_tuple("{:.2f}",    3.14159, std::string("3.14")),
        std::make_tuple("{:.0f}",    3.14159, std::string("3")),
        std::make_tuple("{:.4f}",    3.14159, std::string("3.1416")),    // 四舍五入
        std::make_tuple("{:10.2f}",  3.14159, std::string("      3.14")),
        std::make_tuple("{:<10.2f}", 3.14159, std::string("3.14      ")),
        std::make_tuple("{:>10.2f}", 3.14159, std::string("      3.14")),
        std::make_tuple("{:.2f}",   -3.14159, std::string("-3.14"))
    ));

TEST(FormatFloat, FixedWithInt) {
    // int 走 fixed 时精度无效
    EXPECT_EQ(Terminal::formatString("{:10f}", 42), "        42");
    EXPECT_EQ(Terminal::formatString("{:f}",   42), "42");
}

TEST(FormatFloat, FixedWithBool) {
    // bool 特化：忽略宽度/精度/对齐，直接输出
    EXPECT_EQ(Terminal::formatString("{:f}",   true),  "1");
    EXPECT_EQ(Terminal::formatString("{:f}",   false), "0");
}

// ============================================================================
// 4. 科学计数（e）
// ============================================================================
TEST(FormatFloat, Scientific) {
    EXPECT_EQ(Terminal::formatString("{:e}",     12345.678), "1.234568e+04");
    EXPECT_EQ(Terminal::formatString("{:.2e}",   12345.678), "1.23e+04");
    EXPECT_EQ(Terminal::formatString("{:12.3e}", 12345.678), "   1.235e+04");
    EXPECT_EQ(Terminal::formatString("{:<12.3e}",12345.678), "1.235e+04   ");
}

TEST(FormatFloat, ScientificWithInt) {
    // NOTE: std::scientific 只影响浮点，int 仍走默认输出
    EXPECT_EQ(Terminal::formatString("{:e}", 42), "42");
}

// ============================================================================
// 5. 默认浮点（.precision，无类型字符）
// ============================================================================
TEST(FormatFloat, DefaultFloat) {
    EXPECT_EQ(Terminal::formatString("{:.3}",   3.14159), "3.14");
    EXPECT_EQ(Terminal::formatString("{:.0}",   3.14159), "3");
    EXPECT_EQ(Terminal::formatString("{:10.3}", 3.14159), "      3.14");
    EXPECT_EQ(Terminal::formatString("{:<10.3}",3.14159), "3.14      ");
}

// ============================================================================
// 6. 重复输出（c）
// ============================================================================
TEST(FormatCharRepeat, CharAndBool) {
    EXPECT_EQ(Terminal::formatString("{:c}",  'A'),   "A");
    EXPECT_EQ(Terminal::formatString("{:5c}", 'A'),   "AAAAA");
    EXPECT_EQ(Terminal::formatString("{:3c}", '='),   "===");
    EXPECT_EQ(Terminal::formatString("{:4c}", true),  "1111");
    EXPECT_EQ(Terminal::formatString("{:4c}", false), "0000");
}

TEST(FormatCharRepeat, OtherTypesNoOutput) {
    // NOTE: 主模板 handleCharRepeat 为空，其他类型无输出
    EXPECT_EQ(Terminal::formatString("{:3c}", 42),     "");
    EXPECT_EQ(Terminal::formatString("{:3c}", "abc"),  "");
    EXPECT_EQ(Terminal::formatString("{:3c}", 3.14),   "");
}

// ============================================================================
// 7. 进制格式（#）—— 参数化
// ============================================================================
class BaseIntTest : public ::testing::TestWithParam<
    std::tuple<const char*, int, std::string>> {};

TEST_P(BaseIntTest, Base) {
    EXPECT_EQ(Terminal::formatString(std::get<0>(GetParam()),
                                     std::get<1>(GetParam())),
              std::get<2>(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(
    Ints, BaseIntTest,
    ::testing::Values(
        std::make_tuple("{:#b}", 10,  std::string("00001010")),
        std::make_tuple("{:#b}", 0,   std::string("0")),
        std::make_tuple("{:#b}", 255, std::string("11111111")),
        std::make_tuple("{:#o}", 10,  std::string("12")),
        std::make_tuple("{:#o}", 8,   std::string("10")),
        std::make_tuple("{:#d}", 10,  std::string("10")),
        std::make_tuple("{:#x}", 255, std::string("ff")),
        std::make_tuple("{:#x}", 16,  std::string("10")),
        std::make_tuple("{:#X}", 255, std::string("FF")),
        std::make_tuple("{:#X}", 16,  std::string("10"))
    ));

TEST(FormatBase, Char) {
    EXPECT_EQ(Terminal::formatString("{:#b}", 'A'), "01000001");   // 8 位补零
    EXPECT_EQ(Terminal::formatString("{:#o}", 'A'), "101");
    EXPECT_EQ(Terminal::formatString("{:#d}", 'A'), "65");
    EXPECT_EQ(Terminal::formatString("{:#x}", 'A'), "41");
}

TEST(FormatBase, Bool) {
    EXPECT_EQ(Terminal::formatString("{:#b}", true),  "1");
    EXPECT_EQ(Terminal::formatString("{:#b}", false), "0");
    EXPECT_EQ(Terminal::formatString("{:#o}", true),  "1");
    EXPECT_EQ(Terminal::formatString("{:#o}", false), "0");
    EXPECT_EQ(Terminal::formatString("{:#d}", true),  "1");
    EXPECT_EQ(Terminal::formatString("{:#d}", false), "0");
    EXPECT_EQ(Terminal::formatString("{:#x}", true),  "1");
    EXPECT_EQ(Terminal::formatString("{:#X}", false), "0");
}

// ============================================================================
// 8. 嵌套宽度 / 精度
// ============================================================================
TEST(FormatNested, DynamicWidth) {
    EXPECT_EQ(Terminal::formatString("{:{}s}",  5, 42),     "   42");
    EXPECT_EQ(Terminal::formatString("{:<{}s}", 5, 42),     "42   ");
    EXPECT_EQ(Terminal::formatString("{:>{}s}", 5, 42),     "   42");
    EXPECT_EQ(Terminal::formatString("{:{}s}",  3, "abc"),  "abc");
    EXPECT_EQ(Terminal::formatString("{:{}s}",  6, "abc"),  "   abc");
}

TEST(FormatNested, DynamicPrecision) {
    EXPECT_EQ(Terminal::formatString("{:.{}f}", 2, 3.14159),  "3.14");
    EXPECT_EQ(Terminal::formatString("{:.{}f}", 0, 3.14159),  "3");
    EXPECT_EQ(Terminal::formatString("{:{}f}",  10, 3.14159), "  3.141590");
}

TEST(FormatNested, DynamicWidthAndPrecision) {
    EXPECT_EQ(Terminal::formatString("{:{}.{}f}",  8, 2, 3.14159), "    3.14");
    EXPECT_EQ(Terminal::formatString("{:<{}.{}f}", 8, 2, 3.14159), "3.14    ");
}

// ============================================================================
// 9. 边界与已知行为
// ============================================================================
TEST(FormatEdge, WidthIgnoredWithoutTypeOrPrecision) {
    // NOTE: 无类型字符且无小数点时，宽度被忽略
    EXPECT_EQ(Terminal::formatString("{:}",   42), "42");
    EXPECT_EQ(Terminal::formatString("{:10}", 42), "42");
    EXPECT_EQ(Terminal::formatString("{:<10}",42), "42");
    EXPECT_EQ(Terminal::formatString("{:>10}",42), "42");
    EXPECT_EQ(Terminal::formatString("{:10}", 3.14), "3.14");
}

TEST(FormatEdge, ZeroValue) {
    EXPECT_EQ(Terminal::formatString("{:#b}", 0), "0");
    EXPECT_EQ(Terminal::formatString("{:#o}", 0), "0");
    EXPECT_EQ(Terminal::formatString("{:#x}", 0), "0");
    EXPECT_EQ(Terminal::formatString("{:5s}",  0), "    0");
}

TEST(FormatEdge, NegativeInt) {
    EXPECT_EQ(Terminal::formatString("{:#d}", -42), "-42");
    EXPECT_EQ(Terminal::formatString("{:5s}",  -42), "  -42");
}

TEST(FormatEdge, Mixed) {
    EXPECT_EQ(Terminal::formatString("[{:<6s}] = {:.2f}", "pi", 3.14159),
              "[pi    ] = 3.14");
    EXPECT_EQ(Terminal::formatString("{} -> {:#x} -> {:.2f}", 255, 255, 2.555),
              "255 -> ff -> 2.56");
}

// ============================================================================
// 10. 填充字符（fill）
// ============================================================================
// 语法：{:*[fill][align][width][.precision][type]}
//   - '*' 表示进入"填充段"
//   - [fill] 为单个填充字符；若 '*' 后紧跟 '<' / '>' / '^' 则视为省略，默认空格
//   - [align] ∈ { '<', '>', '^' }，可省略（省略时沿用原有默认：右对齐）
//   - [width] / [.precision] / [type] 语义与普通格式一致

// -------- 10.1 字符串填充 --------
class FillCharStringTest : public ::testing::TestWithParam<
    std::tuple<const char*, std::string, std::string>> {};

TEST_P(FillCharStringTest, Fill) {
    EXPECT_EQ(Terminal::formatString(std::get<0>(GetParam()),
                                     std::get<1>(GetParam())),
              std::get<2>(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(
    Strings, FillCharStringTest,
    ::testing::Values(
        // 左对齐
        std::make_tuple("{:*_<6s}",  std::string("abc"), std::string("abc___")),
        std::make_tuple("{:*0<6s}",  std::string("abc"), std::string("abc000")),
        std::make_tuple("{:**<6s}",  std::string("abc"), std::string("abc***")),
        // 右对齐
        std::make_tuple("{:*_>6s}",  std::string("abc"), std::string("___abc")),
        std::make_tuple("{:*0>6s}",  std::string("abc"), std::string("000abc")),
        std::make_tuple("{:*x>5s}",  std::string("abc"), std::string("xxabc")),
        // 居中（用能整除的填充量，结果与"左多/右多"策略无关）
        std::make_tuple("{:*_^5s}",  std::string("abc"), std::string("_abc_")),
        std::make_tuple("{:*_^7s}",  std::string("abc"), std::string("__abc__")),
        std::make_tuple("{:*x^5s}",  std::string("abc"), std::string("xabcx")),
        // 显式以空格作填充字符
        std::make_tuple("{:* >6s}",  std::string("abc"), std::string("   abc")),
        // 省略 align：沿用原有默认（右对齐）
        std::make_tuple("{:*_6s}",   std::string("abc"), std::string("___abc"))
    ));

TEST(FormatFill, StringWithoutFill) {
    // 未加 '*' 标记：走原有空格填充，作为对照
    EXPECT_EQ(Terminal::formatString("{:>5s}", "abc"), "  abc");
    EXPECT_EQ(Terminal::formatString("{:<5s}", "abc"), "abc  ");
    EXPECT_EQ(Terminal::formatString("{:^5s}", "abc"), " abc ");
}

// -------- 10.2 整型填充 --------
TEST(FormatFill, Int) {
    EXPECT_EQ(Terminal::formatString("{:*_>6s}",  42), "____42");
    EXPECT_EQ(Terminal::formatString("{:*_<6s}",  42), "42____");
    EXPECT_EQ(Terminal::formatString("{:*_^6s}",  42), "__42__");
    EXPECT_EQ(Terminal::formatString("{:*0>5s}",  42), "00042");
    // 负号属于内容的一部分，不参与填充
    EXPECT_EQ(Terminal::formatString("{:**>5s}", -42), "**-42");
    EXPECT_EQ(Terminal::formatString("{:*=<5s}",  42), "42===");
}

// -------- 10.3 浮点 / 科学计数填充 --------
class FillCharFloatTest : public ::testing::TestWithParam<
    std::tuple<const char*, double, std::string>> {};

TEST_P(FillCharFloatTest, Fill) {
    EXPECT_EQ(Terminal::formatString(std::get<0>(GetParam()),
                                     std::get<1>(GetParam())),
              std::get<2>(GetParam()));
}

INSTANTIATE_TEST_SUITE_P(
    Floats, FillCharFloatTest,
    ::testing::Values(
        // 定点小数：内容 "3.14" 长 4
        std::make_tuple("{:*_>10.2f}",  3.14159,  std::string("______3.14")),
        std::make_tuple("{:*_<10.2f}",  3.14159,  std::string("3.14______")),
        std::make_tuple("{:*_^10.2f}",  3.14159,  std::string("___3.14___")),
        std::make_tuple("{:*0>10.2f}",  3.14159,  std::string("0000003.14")),
        // 负数：内容 "-3.14" 长 5
        std::make_tuple("{:*_>10.2f}", -3.14159,  std::string("_____-3.14")),
        // 科学计数：内容 "1.235e+04" 长 9
        std::make_tuple("{:*_>13.3e}", 12345.678, std::string("____1.235e+04")),
        std::make_tuple("{:*_<13.3e}", 12345.678, std::string("1.235e+04____")),
        std::make_tuple("{:*_^13.3e}", 12345.678, std::string("__1.235e+04__")),
        // 默认浮点（仅精度）：内容 "3.14" 长 4
        std::make_tuple("{:*_>10.3}",   3.14159,  std::string("______3.14"))
    ));

// -------- 10.4 填充 + 动态宽度 / 精度 --------
TEST(FormatFill, DynamicWidthPrecision) {
    // 动态宽度
    EXPECT_EQ(Terminal::formatString("{:* >{}s}", 5, "abc"), "  abc");
    EXPECT_EQ(Terminal::formatString("{:*_>{}s}", 5, "abc"), "__abc");
    EXPECT_EQ(Terminal::formatString("{:*_<{}s}", 5, "abc"), "abc__");
    EXPECT_EQ(Terminal::formatString("{:*0>{}s}", 6, "abc"), "000abc");
    EXPECT_EQ(Terminal::formatString("{:*x^{}s}", 5, "abc"), "xabcx");
    // 动态宽度 + 精度
    EXPECT_EQ(Terminal::formatString("{:*_>{}.2f}",  10, 3.14159), "______3.14");
    EXPECT_EQ(Terminal::formatString("{:*_<{}.{}f}", 10, 2, 3.14159),
              "3.14______");
    EXPECT_EQ(Terminal::formatString("{:*0>{}.{}f}", 10, 2, 3.14159),
              "0000003.14");
}

// -------- 10.5 边界与已知行为 --------
TEST(FormatFill, EdgeCases) {
    // 宽度 <= 内容长度：不填充
    EXPECT_EQ(Terminal::formatString("{:*_>2s}", "abc"),   "abc");
    EXPECT_EQ(Terminal::formatString("{:*_>5s}", "abcde"), "abcde");

    // 空字符串：全部填充
    EXPECT_EQ(Terminal::formatString("{:*_>3s}", ""), "___");
    EXPECT_EQ(Terminal::formatString("{:*_<3s}", ""), "___");
    EXPECT_EQ(Terminal::formatString("{:*0>4s}", ""), "0000");

    // 填充字符为特殊符号
    EXPECT_EQ(Terminal::formatString("{:*#>5s}", "ab"), "###ab");
    EXPECT_EQ(Terminal::formatString("{:**>5s}", "ab"), "***ab");

    // 填充字符 = 内容字符：结果拼接为纯填充串
    EXPECT_EQ(Terminal::formatString("{:*a>5s}", "a"), "aaaaa");
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
