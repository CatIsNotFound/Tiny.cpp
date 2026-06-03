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

#include "CommandParser.hpp"
#include <sstream>
#include <iomanip>

namespace Tiny {
    CommandParser::CommandParser(int argc, char** argv) : _argc(argc), _argv(argv) {}

    bool CommandParser::addCommand(const std::string &command_name, const std::string &short_options, const std::string &desciption, 
                            bool has_value, const std::string &default_value, bool default_command) {
        if (exist(command_name)) return false;
        Command command;
        command.option_name = command_name;
        command.short_options = short_options;
        command.description = desciption;
        command.has_value = has_value;
        command.default_value = default_value;
        command.is_default_command = default_command;
        _commands.emplace(command_name, command);
        return true;
    }

    bool CommandParser::addFullCommand(const std::string &command_name, const std::string &description, bool has_value, const std::string &default_value, bool default_command) {
        if (exist(command_name)) return false;
        Command command;
        command.option_name = command_name;
        command.description = description;
        command.has_value = has_value;
        command.default_value = default_value;
        command.is_default_command = default_command;
        command.full_option_only = true;
        _commands.emplace(command_name, command);
        return true;
    }   

    bool CommandParser::remove(const std::string &command_name) {
        if (_commands.find(command_name) != _commands.end()) {
            _commands.erase(command_name);
            return true;
        }
        return false;
    }

    void CommandParser::clear() {
        _commands.clear();
    }

    CommandParser::ParseError CommandParser::exec(int* parsed_command_count, int* err_arg_n, const char* err_cmd_name) {
        // Parsed all of the arguments and convert the commands.
        int err_pos;
        auto err = parseUserCommand(err_pos);
        if (err != ParseError::NoError) {
            if (parsed_command_count) *parsed_command_count = _exec_cmd_list.size();
            if (err_arg_n) *err_arg_n = err_pos;
            return err;
        }
        if (parsed_command_count) *parsed_command_count = _exec_cmd_list.size();
        // Execute the commands
        std::string default_command_name;
        for (auto& cmd : _exec_cmd_list) {
            if (cmd.is_default_command) default_command_name = cmd.option_name;
            if (!cmd.has_value) continue;
            if (cmd.value.empty()) {
                if (cmd.default_value.empty()) {
                    err_cmd_name = cmd.option_name.c_str();
                    return ParseError::InvalidValue;
                } else {
                    cmd.value = cmd.default_value;
                }
            }
        }
        return ParseError::NoError;
    }

    const std::vector<CommandParser::Command>& CommandParser::execCommandList() const {
        return _exec_cmd_list;
    }


    size_t CommandParser::size() const {
        return _commands.size();
    }

    CommandParser::iter CommandParser::begin() {
        return _commands.begin();
    }

    CommandParser::iter CommandParser::end() {
        return _commands.end();
    }

    CommandParser::constIter CommandParser::cbegin() const {
        return _commands.cbegin();
    }

    CommandParser::constIter CommandParser::cend() const {
        return _commands.cend();
    }

    bool CommandParser::exist(const std::string& command_name) const {
        return _commands.find(command_name) != _commands.end();
    }

    const CommandParser::Command &CommandParser::at(const std::string &command_name) const {
        return _commands.at(command_name);
    }

    CommandParser::Command &CommandParser::get(const std::string &command_name) {
        return _commands[command_name];
    }

    CommandParser::Command &CommandParser::operator[](const std::string &command_name) {
        return _commands[command_name];
    }

    CommandParser::ParseError CommandParser::parseUserCommand(int& err_pos) {
        if (!_exec_cmd_list.empty()) _exec_cmd_list.clear();
        std::string filling_option;
        bool is_parsing = false;
        for (size_t i = 1; i < _argc; ++i) {
            std::string arg = _argv[i];
            // Parse the first argument.
            if (i == 1 && arg[0] != '-') {
                bool is_ok = false;
                for (auto& command : _commands) {
                    if (!command.second.full_option_only) continue;
                    if (arg == command.second.option_name) {
                        is_ok = true;
                        if (command.second.has_value) {
                            is_parsing = true;
                            filling_option = arg;
                            _exec_cmd_list.push_back(command.second);
                        } else {
                            _exec_cmd_list.push_back(command.second);
                            break;
                        }
                    }
                }
                if (is_ok) continue;
                else {
                    // Error, This command is not found!
                    err_pos = i;
                    return ParseError::UnknownOption;
                }
            }
            // Fill in the value
            else if (is_parsing && !filling_option.empty()) {
                if (exist(filling_option)) {
                    _exec_cmd_list.back().value = arg;
                    is_parsing = false;
                    filling_option.clear();
                } else {
                    // Error, command is unknown
                    err_pos = i;
                    return ParseError::UnknownOption;
                }
            }
            // Full option name
            else if (arg[0] == '-' && arg[1] == '-') {
                size_t has_equal = arg.find_first_of('=');
                // If parsing "--option" (without value)
                if (has_equal == std::string::npos) {
                    arg = arg.substr(2);
                    auto key_arg = _commands.find(arg);
                    if (key_arg == _commands.end()) {
                        // Error, can not found command!
                        err_pos = i;
                        return ParseError::UnknownOption;
                    } else if (key_arg->second.full_option_only) {
                        err_pos = i;
                        return ParseError::FullOptionOnly;
                    } else if (key_arg->second.has_value) {
                        if (i == _argc - 1) {
                            err_pos = i;
                            return ParseError::MissingArgument;
                        } else {
                            is_parsing = true;
                            filling_option = key_arg->second.option_name;
                        }
                    }
                    _exec_cmd_list.push_back(_commands[arg]);
                    continue;
                } else {
                    // Parsing "--option=value"
                    auto key_option = arg.substr(2, has_equal - 2);
                    auto value = arg.substr(has_equal + 1);
                    auto found_cmd = _commands.find(key_option);
                    if (found_cmd == _commands.end()) {
                        // Error, can not found command!
                        err_pos = i;
                        return ParseError::UnknownOption;
                    } else if (found_cmd->second.full_option_only) {
                        err_pos = i;
                        return ParseError::FullOptionOnly;
                    }
                    _exec_cmd_list.push_back(_commands[key_option]);
                    _exec_cmd_list.back().value = value;
                }
                continue;
            }
            // Short option name
            else if (arg[0] == '-') {
                is_parsing = true;
                bool is_value = false;
                bool is_match_opt;
                bool is_all_match = true;
                for (size_t j = 1; j < arg.size(); ++j) {
                    char opt = arg[j];
                    is_match_opt = false;
                    for (auto& item : _commands) {
                        auto& cmd = item.second;
                        if (cmd.short_options.find(opt) != std::string::npos) {
                            is_match_opt = true;
                            filling_option = cmd.option_name;
                            _exec_cmd_list.push_back(cmd);
                            if (j == arg.size() - 1 && cmd.has_value) {
                                
                                // Needs value of the arguments
                                err_pos = i;
                                return ParseError::MissingArgument;
                            } else if (arg.size() > j + 1 && cmd.has_value) {
                                size_t count = arg.size() - j - 1;
                                _exec_cmd_list.back().value = arg.substr(arg[j + 1] == '=' ? j + 2 : j + 1);
                                filling_option.clear();
                                is_parsing = false;
                            } else if (!cmd.has_value) {
                                is_parsing = false;
                                filling_option.clear();
                            }
                        }
                        if (is_match_opt) break;
                    }
                    if (!is_match_opt) {
                        // One of the short option is unknown!
                        err_pos = i;
                        return ParseError::UnknownOption;
                    }
                }
            }
            else {
                err_pos = i;
                return ParseError::FormatError;
            }
        }

        // Is parsing completely?
        if (is_parsing) {
            err_pos = _argc - 1;
            return ParseError::MissingArgument;
        }

        // All of the arguments has no problem!
        err_pos = 0;
        return ParseError::NoError;
    }
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