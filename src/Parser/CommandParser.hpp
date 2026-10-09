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

#ifndef TINY_CPP_COMMAND_PARSER_HPP
#define TINY_CPP_COMMAND_PARSER_HPP
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cstdint>

#if defined(__clang__) || defined(__GNUC__)
#   define API_DEPRECATED(msg) __attribute__((deprecated(msg)))
#elif defined(_MSC_VER)
#   define API_DEPRECATED(msg) __declspec(deprecated(msg))
#else
#   define API_DEPRECATED(msg)
#endif


namespace Tiny {
    class CommandParser {
    public:
        struct Command {
            std::string option_name{};
            std::string short_options{};
            std::string description{};
            bool full_option_only{false};
            bool is_default_command{false};
            bool is_required{false};
            bool has_value{false};
            // If set to `true`, all parsing will stop (ignore all the required commands) after this command is parsed.
            bool is_last_command{false};
            std::string value{};
            std::string default_value{};
            std::string value_placeholder{"VAL"};

            Command() = default;
            Command(const Command& other) = default;
            Command& operator=(const Command& other) = default;
        };

        enum class ParseError : uint8_t {
            NoError,
            UnknownOption,
            FullOptionOnly,
            InvalidValue,
            MissingArgument,
            FormatError,
            MissingDefaultCommand
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
                case ParseError::MissingDefaultCommand:
                    return "Missing default command";
                default:
                    return "Unknown error";
            }
        }

        using iter = std::unordered_map<std::string, Command>::iterator;
        using constIter = std::unordered_map<std::string, Command>::const_iterator;
        
        CommandParser(int argc, char** argv);
        ~CommandParser() = default;
        bool addCommand(const std::string& command_name, const std::string& short_options, const std::string& description = {},
                 bool has_value = false, const std::string& default_value = {},
                 bool is_required = false, bool default_command = false, const std::string& placeholder = "VAL");
        bool addFullCommand(const std::string& command_name, const std::string& description, 
                 bool has_value = false, const std::string& default_value = {},
                 bool is_required = false, bool default_command = false, const std::string& placeholder = "VAL");
        bool addLastCommand(const std::string& command_name, const std::string& short_options, const std::string& description = {},
                 bool has_value = false, const std::string& default_value = {},
                 const std::string& placeholder = "VAL");
        bool addFullLastCommand(const std::string& command_name, const std::string& description = {},
                 bool has_value = false, const std::string& default_value = {},
                 const std::string& placeholder = "VAL");
        bool remove(const std::string& command_name);
        void clear();
        ParseError exec(int* parsed_command_count = nullptr,
                        int* err_arg_n = nullptr,
                        std::vector<std::string>* missing_command_list = nullptr);
        const std::vector<Command>& execCommandList() const;
        std::string generateHelpInfo(uint32_t max_width = 64, bool sort_option_name = false,
                                     bool show_options_only = false) const;

        bool renameCommand(const std::string &command_name, const std::string &new_name);
        const std::string& lastCommandName() const;
        size_t size() const;
        iter begin();
        iter end();
        constIter cbegin() const;
        constIter cend() const;
        bool exist(const std::string& command_name) const;
        const Command& at(const std::string& command_name) const;
        API_DEPRECATED("This function will not be available since next version, please don't use it!")
        Command& get(const std::string& command_name);
        API_DEPRECATED("This function will not be available since next version, please don't use it!")
        Command& operator[](const std::string& command_name);
    private:
        ParseError parseUserCommand(int& err_pos, std::vector<std::string> &missing_command_list);
        bool checkAndRemoveRequiredCommand(std::vector<std::string>& required_cmd_list, const std::string& command_name);
        std::string makeShortOptions(const std::string& short_options);
        std::string makeOptionName(const std::string& short_options);
        std::unordered_map<std::string, Command> _commands;
        std::vector<Command> _exec_cmd_list;
        std::vector<std::string> _required_cmd_list;
        std::string _default_cmd, _last_cmd_name{};
        char** _argv;
        int _argc;
    };
}



#endif // TINY_CPP_COMMAND_PARSER_HPP

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