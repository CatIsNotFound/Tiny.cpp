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

#ifndef TINY_CPP_PARSER_COMMANDPARSER_HPP
#define TINY_CPP_PARSER_COMMANDPARSER_HPP
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cstdint>

namespace Tiny {
    class CommandParser {
    public:
        struct Command {
            std::string option_name{};
            std::string short_options{};
            std::string description{};
            bool full_option_only{false};
            bool is_default_command{false};
            bool has_value{false};
            std::string value{};
            std::string default_value{};

            Command() {}
            Command(const Command& other) : option_name(other.option_name), short_options(other.short_options), 
                                            description(other.description), full_option_only(other.full_option_only),
                                            is_default_command(other.is_default_command), has_value(other.has_value),
                                            value(other.value), default_value(other.default_value) {}
            Command& operator=(const Command& other) {
                option_name = other.option_name;
                short_options = other.short_options;
                description = other.description;
                full_option_only = other.full_option_only;
                is_default_command = other.is_default_command;
                has_value = other.has_value;
                value = other.value;
                default_value = other.default_value;
                return *this;
            }
        };

        enum class ParseError : uint8_t {
            NoError,
            UnknownOption,
            FullOptionOnly,
            InvalidValue,
            MissingArgument,
            FormatError
        };

        static const char* getParseErrorName(ParseError error) {
            switch (error) {
                case ParseError::NoError:
                    return "Successful";
                case ParseError::UnknownOption:
                    return "Unknown option";
                case ParseError::FullOptionOnly:
                    return "Full option only";
                case ParseError::InvalidValue:
                    return "Invalid value";
                case ParseError::MissingArgument:
                    return "Missing argument";
                case ParseError::FormatError:
                    return "Format error";
                default:
                    return "Unknown error";
            }
        }

        using iter = std::unordered_map<std::string, Command>::iterator;
        using constIter = std::unordered_map<std::string, Command>::const_iterator;
        
        CommandParser(int argc, char** argv);
        ~CommandParser() = default;
        bool addCommand(const std::string& command_name, const std::string& short_options, const std::string& desciption = {}, 
                 bool has_value = false, const std::string& default_value = {}, bool default_command = false);
        bool addFullCommand(const std::string& command_name, const std::string& description, 
                 bool has_value = false, const std::string& default_value = {}, bool default_command = false);
        bool remove(const std::string& command_name);
        void clear();
        ParseError exec(int* parsed_command_count = nullptr,
                        int* err_arg_n = nullptr,
                        const char* err_cmd_name = nullptr);
        const std::vector<Command>& execCommandList() const;

        size_t size() const;
        iter begin();
        iter end();
        constIter cbegin() const;
        constIter cend() const;
        bool exist(const std::string& command_name) const;
        const Command& at(const std::string& command_name) const;
        Command& get(const std::string& command_name);
        Command& operator[](const std::string& command_name);
    private:
        ParseError parseUserCommand(int& err_pos);
        std::unordered_map<std::string, Command> _commands;
        std::vector<Command> _exec_cmd_list;
        char** _argv;
        int _argc;
    };
}



#endif // TINY_CPP_PARSER_COMMANDPARSER_HPP

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