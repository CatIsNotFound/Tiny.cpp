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
#include <algorithm>
#include "../src/Parser/IniParser.hpp"

using namespace Tiny;

// ==================== Constructor Tests ====================

TEST(IniParserConstructor, DefaultConstructor) {
    IniParser parser;
    EXPECT_EQ(parser.currentGroupName(), "ungrouped");
    EXPECT_EQ(parser.groupsCount(), 1u);
    EXPECT_EQ(parser.keysCount(), 0u);
}

TEST(IniParserConstructor, NamedGroupConstructor) {
    IniParser parser("mygroup");
    EXPECT_EQ(parser.currentGroupName(), "mygroup");
    EXPECT_EQ(parser.groupsCount(), 1u);
    EXPECT_EQ(parser.keysCount(), 0u);
}

// ==================== Parse: Basic Key-Value ====================

TEST(IniParserParse, EmptyString) {
    IniParser parser;
    EXPECT_EQ(parser.parse(""), IniParserError::Success);
}

TEST(IniParserParse, SingleKeyValue) {
    IniParser parser;
    EXPECT_EQ(parser.parse("key = value"), IniParserError::Success);
    bool ok = false;
    EXPECT_EQ(parser.value("key", false, &ok), "value");
    EXPECT_TRUE(ok);
}

TEST(IniParserParse, MultipleKeyValues) {
    IniParser parser;
    EXPECT_EQ(parser.parse("key1 = val1\nkey2 = val2\nkey3 = val3"), IniParserError::Success);
    EXPECT_EQ(parser.keysCount(), 3u);
    EXPECT_EQ(parser.value("key1", false), "val1");
    EXPECT_EQ(parser.value("key2", false), "val2");
    EXPECT_EQ(parser.value("key3", false), "val3");
}

TEST(IniParserParse, WhitespaceStripping) {
    IniParser parser;
    EXPECT_EQ(parser.parse("  key  =  value  "), IniParserError::Success);
    EXPECT_EQ(parser.value("key", false), "value");
}

TEST(IniParserParse, CommentLinesSkipped) {
    IniParser parser;
    EXPECT_EQ(parser.parse("; this is a comment\nkey = value"), IniParserError::Success);
    EXPECT_EQ(parser.keysCount(), 1u);
    EXPECT_EQ(parser.value("key", false), "value");
}

TEST(IniParserParse, BlankLinesSkipped) {
    IniParser parser;
    EXPECT_EQ(parser.parse("\n\nkey = value\n\n"), IniParserError::Success);
    EXPECT_EQ(parser.keysCount(), 1u);
}

TEST(IniParserParse, ParseWithCharPointer) {
    IniParser parser;
    const char* data = "key = value";
    EXPECT_EQ(parser.parse(data, strlen(data)), IniParserError::Success);
    EXPECT_EQ(parser.value("key", false), "value");
}

// ==================== Parse: Groups ====================

TEST(IniParserParse, SingleGroup) {
    IniParser parser;
    EXPECT_EQ(parser.parse("[section]\nkey = value"), IniParserError::Success);
    EXPECT_EQ(parser.currentGroupName(), "section");
    EXPECT_EQ(parser.value("key", false), "value");
}

TEST(IniParserParse, MultipleGroups) {
    IniParser parser;
    std::string input = "[group1]\nkey1 = val1\n[group2]\nkey2 = val2";
    EXPECT_EQ(parser.parse(input), IniParserError::Success);
    EXPECT_EQ(parser.groupsCount(), 2u);

    parser.setGroup("group1");
    EXPECT_EQ(parser.value("key1", false), "val1");
    EXPECT_EQ(parser.keysCount(), 1u);

    parser.setGroup("group2");
    EXPECT_EQ(parser.value("key2", false), "val2");
    EXPECT_EQ(parser.keysCount(), 1u);
}

TEST(IniParserParse, GroupWithWhitespace) {
    IniParser parser;
    EXPECT_EQ(parser.parse("  [section]  \nkey = value"), IniParserError::Success);
    EXPECT_EQ(parser.currentGroupName(), "section");
}

// ==================== Parse: Error Cases ====================

TEST(IniParserParseError, MissingClosingBracket) {
    IniParser parser;
    EXPECT_EQ(parser.parse("[section"), IniParserError::InvalidFormat);
}

TEST(IniParserParseError, GroupNameContainsBracket) {
    IniParser parser;
    EXPECT_EQ(parser.parse("[sec[tion]"), IniParserError::InvalidCharacter);
}

TEST(IniParserParseError, GroupNameContainsEquals) {
    IniParser parser;
    EXPECT_EQ(parser.parse("[sec=tion]"), IniParserError::InvalidCharacter);
}

TEST(IniParserParseError, KeyContainsBracket) {
    IniParser parser;
    EXPECT_EQ(parser.parse("k[ey = value"), IniParserError::InvalidCharacter);
}

TEST(IniParserParseError, ValueContainsBracket) {
    IniParser parser;
    EXPECT_EQ(parser.parse("key = v[alue"), IniParserError::InvalidCharacter);
}

TEST(IniParserParseError, ValueContainsEquals) {
    IniParser parser;
    EXPECT_EQ(parser.parse("key = val=ue"), IniParserError::InvalidCharacter);
}

TEST(IniParserParseError, EmptyKey) {
    IniParser parser;
    EXPECT_EQ(parser.parse("= value"), IniParserError::InvalidFormat);
}

TEST(IniParserParseError, NoEqualsNoBracket) {
    IniParser parser;
    EXPECT_EQ(parser.parse("justtext"), IniParserError::InvalidFormat);
}

TEST(IniParserParseError, BothBracketAndEquals) {
    IniParser parser;
    EXPECT_EQ(parser.parse("[section] = value"), IniParserError::InvalidFormat);
}

TEST(IniParserParseError, ErrorResetsGroupToUngrouped) {
    IniParser parser;
    parser.parse("[section]\nkey = value");
    parser.parse("[bad\nkey = value");
    EXPECT_EQ(parser.currentGroupName(), "ungrouped");
}

// ==================== setValue / value ====================

TEST(IniParserSetValue, SetAndGet) {
    IniParser parser;
    std::string val = "hello";
    parser.setValue("key", val);
    bool ok = false;
    EXPECT_EQ(parser.value("key", false, &ok), "hello");
    EXPECT_TRUE(ok);
}

TEST(IniParserSetValue, OverwriteExisting) {
    IniParser parser;
    std::string v1 = "first";
    std::string v2 = "second";
    parser.setValue("key", v1);
    parser.setValue("key", v2);
    EXPECT_EQ(parser.value("key", false), "second");
    EXPECT_EQ(parser.keysCount(), 1u);
}

TEST(IniParserSetValue, SetEmptyValue) {
    IniParser parser;
    std::string val = "";
    parser.setValue("key", val);
    bool ok = false;
    EXPECT_EQ(parser.value("key", false, &ok), "");
    EXPECT_TRUE(ok);
    EXPECT_TRUE(parser.isKey("key"));
}

TEST(IniParserSetValue, SetMultipleKeys) {
    IniParser parser;
    std::string v1 = "val1", v2 = "val2", v3 = "val3";
    parser.setValue("k1", v1);
    parser.setValue("k2", v2);
    parser.setValue("k3", v3);
    EXPECT_EQ(parser.keysCount(), 3u);
    EXPECT_EQ(parser.value("k1", false), "val1");
    EXPECT_EQ(parser.value("k2", false), "val2");
    EXPECT_EQ(parser.value("k3", false), "val3");
}

TEST(IniParserSetValue, OverwriteMultipleTimes) {
    IniParser parser;
    std::string v1 = "a", v2 = "b", v3 = "c";
    parser.setValue("key", v1);
    parser.setValue("key", v2);
    parser.setValue("key", v3);
    EXPECT_EQ(parser.value("key", false), "c");
    EXPECT_EQ(parser.keysCount(), 1u);
}

TEST(IniParserSetValue, SetValueWithSpaces) {
    IniParser parser;
    std::string val = "hello world";
    parser.setValue("key", val);
    EXPECT_EQ(parser.value("key", false), "hello world");
}

TEST(IniParserSetValue, SetValueInDifferentGroups) {
    IniParser parser;
    parser.setGroup("g1");
    std::string v1 = "val1";
    parser.setValue("key", v1);
    parser.setGroup("g2");
    std::string v2 = "val2";
    parser.setValue("key", v2);
    parser.setGroup("g1");
    EXPECT_EQ(parser.value("key", false), "val1");
    parser.setGroup("g2");
    EXPECT_EQ(parser.value("key", false), "val2");
}

TEST(IniParserSetValue, SetAfterParse) {
    IniParser parser;
    parser.parse("key1 = old_val");
    std::string new_val = "new_val";
    parser.setValue("key1", new_val);
    EXPECT_EQ(parser.value("key1", false), "new_val");
    EXPECT_EQ(parser.keysCount(), 1u);
}

TEST(IniParserSetValue, SetNewKeyAfterParse) {
    IniParser parser;
    parser.parse("key1 = val1");
    std::string v2 = "val2";
    parser.setValue("key2", v2);
    EXPECT_EQ(parser.keysCount(), 2u);
    EXPECT_EQ(parser.value("key1", false), "val1");
    EXPECT_EQ(parser.value("key2", false), "val2");
}

TEST(IniParserSetValue, SetSpecialCharactersInValue) {
    IniParser parser;
    std::string val = "special!@#$%^&*()chars";
    parser.setValue("key", val);
    EXPECT_EQ(parser.value("key", false), "special!@#$%^&*()chars");
}

TEST(IniParserSetValue, SetLongValue) {
    IniParser parser;
    std::string val(1000, 'x');
    parser.setValue("key", val);
    EXPECT_EQ(parser.value("key", false), val);
}

TEST(IniParserValue, NonExistentKey) {
    IniParser parser;
    bool ok = true;
    EXPECT_EQ(parser.value("nokey", false, &ok), "");
    EXPECT_FALSE(ok);
}

TEST(IniParserValue, NonExistentKeyWithoutOk) {
    IniParser parser;
    EXPECT_EQ(parser.value("nokey", false), "");
}

TEST(IniParserValue, EscapeSequenceParsing) {
    IniParser parser;
    std::string val = "line1\\nline2";
    parser.setValue("key", val);
    std::string result = parser.value("key", true);
    EXPECT_NE(result, "line1\\nline2");
    EXPECT_NE(result.find('\n'), std::string::npos);
}

TEST(IniParserValue, NoEscapeParsing) {
    IniParser parser;
    std::string val = "line1\\nline2";
    parser.setValue("key", val);
    EXPECT_EQ(parser.value("key", false), "line1\\nline2");
}

TEST(IniParserValue, EscapeTab) {
    IniParser parser;
    std::string val = "col1\\tcol2";
    parser.setValue("key", val);
    std::string result = parser.value("key", true);
    EXPECT_NE(result.find('\t'), std::string::npos);
}

TEST(IniParserValue, EscapeBackslash) {
    IniParser parser;
    std::string val = "path\\\\to\\\\file";
    parser.setValue("key", val);
    std::string result = parser.value("key", true);
    EXPECT_NE(result, "path\\\\to\\\\file");
}

TEST(IniParserValue, EscapeMultipleSequences) {
    IniParser parser;
    std::string val = "a\\nb\\tc\\\\d";
    parser.setValue("key", val);
    std::string result = parser.value("key", true);
    EXPECT_NE(result.find('\n'), std::string::npos);
    EXPECT_NE(result.find('\t'), std::string::npos);
}

TEST(IniParserValue, ValueWithoutEscapes) {
    IniParser parser;
    std::string val = "no escapes here";
    parser.setValue("key", val);
    EXPECT_EQ(parser.value("key", true), "no escapes here");
    EXPECT_EQ(parser.value("key", false), "no escapes here");
}

// ==================== unsetValue ====================

TEST(IniParserUnset, RemoveExistingKey) {
    IniParser parser;
    std::string val = "value";
    parser.setValue("key", val);
    EXPECT_TRUE(parser.isKey("key"));
    parser.unsetValue("key");
    EXPECT_FALSE(parser.isKey("key"));
    EXPECT_EQ(parser.keysCount(), 0u);
}

TEST(IniParserUnset, RemoveNonExistentKey) {
    IniParser parser;
    parser.unsetValue("nokey");
    EXPECT_EQ(parser.keysCount(), 0u);
}

// ==================== Group Operations ====================

TEST(IniParserGroup, SetGroupCreatesNew) {
    IniParser parser;
    parser.setGroup("newgroup");
    EXPECT_EQ(parser.currentGroupName(), "newgroup");
    EXPECT_EQ(parser.groupsCount(), 2u);
}

TEST(IniParserGroup, SetGroupEmpty) {
    IniParser parser;
    parser.setGroup("somegroup");
    parser.setGroup("");
    EXPECT_EQ(parser.currentGroupName(), "ungrouped");
}

TEST(IniParserGroup, RemoveGroup) {
    IniParser parser;
    parser.setGroup("toremove");
    std::string val = "v";
    parser.setValue("k", val);
    EXPECT_EQ(parser.groupsCount(), 2u);
    parser.removeGroup("toremove");
    EXPECT_EQ(parser.groupsCount(), 1u);
}

TEST(IniParserGroup, RemoveCurrentGroupSwitches) {
    IniParser parser;
    parser.setGroup("a");
    parser.setGroup("b");
    parser.removeGroup("b");
    EXPECT_NE(parser.currentGroupName(), "b");
}

TEST(IniParserGroup, RemoveLastGroupResetsToUngrouped) {
    IniParser parser;
    parser.removeGroup("ungrouped");
    EXPECT_EQ(parser.currentGroupName(), "ungrouped");
    EXPECT_EQ(parser.groupsCount(), 1u);
}

TEST(IniParserGroup, GroupsList) {
    IniParser parser;
    parser.setGroup("alpha");
    parser.setGroup("beta");
    auto grps = parser.groups();
    EXPECT_EQ(grps.size(), 3u);
    EXPECT_NE(std::find(grps.begin(), grps.end(), "ungrouped"), grps.end());
    EXPECT_NE(std::find(grps.begin(), grps.end(), "alpha"), grps.end());
    EXPECT_NE(std::find(grps.begin(), grps.end(), "beta"), grps.end());
}

// ==================== keys / keysCount / isKey ====================

TEST(IniParserKeys, KeysList) {
    IniParser parser;
    std::string v = "v";
    parser.setValue("k1", v);
    parser.setValue("k2", v);
    parser.setValue("k3", v);
    auto ks = parser.keys();
    EXPECT_EQ(ks.size(), 3u);
    EXPECT_NE(std::find(ks.begin(), ks.end(), "k1"), ks.end());
    EXPECT_NE(std::find(ks.begin(), ks.end(), "k2"), ks.end());
    EXPECT_NE(std::find(ks.begin(), ks.end(), "k3"), ks.end());
}

TEST(IniParserKeys, IsKey) {
    IniParser parser;
    std::string v = "v";
    parser.setValue("exists", v);
    EXPECT_TRUE(parser.isKey("exists"));
    EXPECT_FALSE(parser.isKey("noexist"));
}

TEST(IniParserKeys, ClearKeysCurrentGroup) {
    IniParser parser;
    std::string v = "v";
    parser.setValue("k1", v);
    parser.setValue("k2", v);
    EXPECT_EQ(parser.keysCount(), 2u);
    parser.clearKeys();
    EXPECT_EQ(parser.keysCount(), 0u);
}

TEST(IniParserKeys, ClearKeysSpecificGroup) {
    IniParser parser;
    parser.setGroup("g1");
    std::string v = "v";
    parser.setValue("k1", v);
    parser.setGroup("g2");
    parser.setValue("k2", v);
    parser.clearKeys("g1");
    EXPECT_EQ(parser.keysCount(), 1u);
    parser.setGroup("g1");
    EXPECT_EQ(parser.keysCount(), 0u);
}

// ==================== operator[] ====================

TEST(IniParserSubscript, CreateAndRead) {
    IniParser parser;
    parser["key"] = "value";
    EXPECT_EQ(parser["key"], "value");
}

TEST(IniParserSubscript, CreateNewKeyWithEmptyValue) {
    IniParser parser;
    auto& ref = parser["newkey"];
    EXPECT_EQ(ref, "");
    EXPECT_TRUE(parser.isKey("newkey"));
}

TEST(IniParserSubscript, OverwriteValue) {
    IniParser parser;
    parser["key"] = "first";
    parser["key"] = "second";
    EXPECT_EQ(parser["key"], "second");
}

// ==================== dump ====================

TEST(IniParserDump, BasicDump) {
    IniParser parser;
    std::string v = "val";
    parser.setValue("key", v);
    std::string output = parser.dump();
    EXPECT_NE(output.find("[ungrouped]"), std::string::npos);
    EXPECT_NE(output.find("key = val"), std::string::npos);
}

TEST(IniParserDump, ExcludeEmptyGroups) {
    IniParser parser;
    parser.setGroup("empty_group");
    parser.setGroup("ungrouped");
    std::string v = "val";
    parser.setValue("key", v);
    std::string withEmpty = parser.dump(true);
    std::string withoutEmpty = parser.dump(false);
    EXPECT_NE(withEmpty.find("[empty_group]"), std::string::npos);
    EXPECT_EQ(withoutEmpty.find("[empty_group]"), std::string::npos);
}

TEST(IniParserDump, RoundTrip) {
    IniParser parser;
    std::string input = "[section]\nkey1 = val1\nkey2 = val2\n";
    ASSERT_EQ(parser.parse(input), IniParserError::Success);
    std::string dumped = parser.dump();
    EXPECT_NE(dumped.find("[section]"), std::string::npos);
    EXPECT_NE(dumped.find("key1 = val1"), std::string::npos);
    EXPECT_NE(dumped.find("key2 = val2"), std::string::npos);
}
