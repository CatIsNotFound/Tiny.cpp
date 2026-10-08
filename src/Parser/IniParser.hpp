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

#ifndef TINY_INI_PARSER_HPP
#define TINY_INI_PARSER_HPP
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace Tiny {
    /// @warning In next version, to use IniParser class, you have to use `Tiny::Ini::IniParser` instead.
    namespace Ini {
        using Conf = std::pair<std::string, std::string>;
        using Group = std::vector<Conf>;
        using Map  = std::unordered_map<std::string, Group>;

        enum class ParseError : uint8_t {
            Success,
            InvalidCharacter,
            InvalidFormat
        };

        class Parser {
        public:
            Parser();
            Parser(const std::string& group_name);
            ~Parser();

            ParseError parse();
            ParseError parse(const char* context, size_t length);
            ParseError parse(const std::string& context);
            std::string dump(bool include_empty_group = true);

            void setGroup(const std::string& group);
            const std::string& currentGroupName() const;
            void removeGroup(const std::string& group = {});

            void setValue(const std::string &key, std::string &value);
            void unsetValue(const std::string& key);
            std::string value(const std::string& key, bool parse_escaped_char = true, bool *ok = nullptr);
            void clearKeys();
            void clearKeys(const std::string &group);

            bool isKey(const std::string& key) const;
            std::vector<std::string> keys() const;
            std::vector<std::string> groups() const;
            size_t keysCount() const;
            size_t groupsCount() const;

            std::string& operator[](const std::string& key);

        private:
            ParseError parseContext(std::string& buf);
            Conf* findConf(const std::string& key);
            Map _ini_map;
            std::string _context;
            std::string _cur_group{"ungrouped"};
        };
    }

    /// @warning Please use `Tiny::Ini::Conf` instead
    /// @deprecated It will be removed in next version.
    using IniConf = Ini::Conf;
    /// @warning Please use `Tiny::Ini::Group` instead
    /// @deprecated It will be removed in next version.
    using IniGroup = Ini::Group;
    /// @warning Please use `Tiny::Ini::Map` instead
    /// @deprecated It will be removed in next version.
    using IniMap = Ini::Map;
    /// @warning Please use `Tiny::Ini::ParseError` instead
    /// @deprecated It will be removed in next version.
    typedef Ini::ParseError IniParserError;
    /// @warning Please use `Tiny::Ini::Parser` instead
    /// @deprecated It will be removed in next version.
    typedef Ini::Parser IniParser;
}


#endif //TINY_INIPARSER_HPP

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