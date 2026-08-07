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

#include "IniParser.hpp"
#include <sstream>
#include <deque>
#include <algorithm>

static bool is_space(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

static bool is_include(const std::string& temp) {
    return  temp.find('[') != std::string::npos ||
            temp.find(']') != std::string::npos ||
            temp.find('=') != std::string::npos;
}

static void strip(std::string &str) {
    if (str.empty()) return;
    size_t st = 0;
    while (st < str.size()) {
        if (!is_space(str[st])) break;
        st++;
    }
    size_t ed = str.size() - 1;
    while (ed > st) {
        if (!is_space(str[ed])) break;
        ed--;
    }
    str = str.substr(st, ed - st + 1);
}

static uint32_t str2UInt(const std::string& buf, uint8_t base = 10, bool* ok = nullptr) {
    if (buf.empty() || base > 16) {
        if (ok) *ok = false;
        return {};
    }
    uint32_t n{}, sum{};
    for (size_t i = 0; i < buf.length(); i++) {
        if (base <= 10) {
            if (buf[i] >= '0' && buf[i] < '0' + base) {
                n += buf[i] - '0';
                sum = sum * base + n;
            } else {
                if (ok) *ok = false;
                return sum;
            }
        } else if (base <= 16) {
            if ((buf[i] >= '0' && buf[i] <= '9')) {
                n += buf[i] - '0';
                sum = sum * base + n;
            } else if (buf[i] >= 'A' && buf[i] < 'A' + base - 10) {
                n += buf[i] - 'A' + 10;
                sum = sum * base + n;
            } else if (buf[i] >= 'a' && buf[i] < 'a' + base - 10) {
                n += buf[i] - 'a' + 10;
                sum = sum * base + n;
            } else {
                if (ok) *ok = false;
                return sum;
            }
        }
    }
    if (ok) *ok = true;
    return sum;
}

static void parseString(std::string& str) {
    size_t st_pos = 0, pos = 0;
    std::deque<std::pair<size_t, std::string>> replace_keys;
    do {
        pos = str.find('\\', st_pos);
        if (pos == std::string::npos) break;
        if (pos + 1 >= str.length()) break;
        auto temp = str.substr(pos, 2);
        char ch{};
        switch (temp[1]) {
            case 'a':
                ch = '\a';
                break;
            case 'b':
                ch = '\b';
                break;
            case 'f':
                ch = '\f';
                break;
            case 'n':
                ch = '\n';
                break;
            case 't':
                ch = '\t';
                break;
            case 'r':
                ch = '\r';
                break;
            case 'v':
                ch = '\v';
                break;
            case '0':
                ch = '\0';
                break;
            case '\\':
                ch = '\\';
                break;
            default:
                break;
        }
        temp.resize(1);
        temp[0] = ch;
        st_pos = pos + 2;
        replace_keys.emplace_front(pos, temp);
    } while (st_pos < str.length());
    for (auto& i : replace_keys) {
        str.replace(i.first, 2, i.second);
    }
}

Tiny::IniParser::IniParser() {
    _ini_map["ungrouped"] = {};
}

Tiny::IniParser::IniParser(const std::string &group_name) {
    _ini_map[group_name] = {};
    _cur_group = group_name;
}

Tiny::IniParser::~IniParser() {}

Tiny::IniParserError Tiny::IniParser::parse() {
    if (!_ini_map.empty()) _ini_map.clear();
    if (_context.empty()) return IniParserError::Success;
    size_t pos = 0, st_pos = 0;
    do {
        pos = _context.find_first_of('\n', st_pos);
        auto sub_str = _context.substr(st_pos, pos - st_pos);
        auto err = parseContext(sub_str);
        if (err != IniParserError::Success) {
            _cur_group = "ungrouped";
            return err;
        }
        st_pos = pos + 1;
    } while (pos != std::string::npos && st_pos < _context.length());
    _cur_group = _ini_map.begin()->first;
    return IniParserError::Success;
}

Tiny::IniParserError Tiny::IniParser::parse(const char *context, size_t length) {
    _context.assign(context, length);
    return parse();
}

Tiny::IniParserError Tiny::IniParser::parse(const std::string &context) {
    _context = context;
    return parse();
}

std::string Tiny::IniParser::dump(bool include_empty_group) {
    std::ostringstream oss;
    for (auto& i : _ini_map) {
        if (!include_empty_group && i.second.empty()) continue;
        oss << "[" << i.first << "]\n";
        for (auto& j : i.second) {
            oss << j.first << " = " << j.second << "\n";
        }
        oss << "\n";
    }
    return oss.str();
}

void Tiny::IniParser::setGroup(const std::string &group) {
    _cur_group = group.empty() ? "ungrouped" : group;
    if (_ini_map.find(group) == _ini_map.end()) {
        _ini_map[group] = {};
    }
}

const std::string & Tiny::IniParser::currentGroupName() const {
    return _cur_group;
}

void Tiny::IniParser::removeGroup(const std::string &group) {
    if (_ini_map.find(group.empty() ? _cur_group : group) != _ini_map.end()) {
        _ini_map.erase(group.empty() ? _cur_group : group);
        if (_ini_map.empty()) {
            _ini_map["ungrouped"] = {};
            _cur_group = "ungrouped";
        } else {
            _cur_group = _ini_map.begin()->first;
        }
    }
}

void Tiny::IniParser::setValue(const std::string &key, std::string &value) {
    auto& group = _ini_map[_cur_group];

    auto iter = findConf(key);
    if (iter) {
        iter->second = value;
    } else {
        group.emplace_back(key, value);
    }
}

void Tiny::IniParser::unsetValue(const std::string &key) {
    auto& group = _ini_map.at(_cur_group);
    auto iter = std::find_if(group.cbegin(), group.cend(), [&key](const IniConf& v) {
        return key == v.first;
    });
    if (iter != group.cend()) {
        group.erase(iter);
    }
}

std::string Tiny::IniParser::value(const std::string &key, bool parse_escaped_char, bool *ok) {
    auto iter = findConf(key);
    if (iter) {
        auto value = iter->second;
        if (parse_escaped_char) {
            parseString(value);
        }
        if (ok) *ok = true;
        return value;
    }
    if (ok) *ok = false;
    return {};
}

void Tiny::IniParser::clearKeys() {
    _ini_map.at(_cur_group).clear();
}

void Tiny::IniParser::clearKeys(const std::string &group) {
    if (_ini_map.find(group) != _ini_map.end()) {
        _ini_map.at(group).clear();
    }
}

bool Tiny::IniParser::isKey(const std::string &key) const {
    auto& group = _ini_map.at(_cur_group);
    auto iter = std::find_if(group.cbegin(), group.cend(), [&key](const IniConf& v) {
        return key == v.first;
    });
    return iter != group.cend();
}

std::vector<std::string> Tiny::IniParser::keys() const {
    std::vector<std::string> ret;
    for (auto& i : _ini_map.at(_cur_group)) {
        ret.push_back(i.first);
    }
    return ret;
}

std::vector<std::string> Tiny::IniParser::groups() const {
    std::vector<std::string> ret;
    for (auto& i : _ini_map) {
        ret.push_back(i.first);
    }
    return ret;
}

size_t Tiny::IniParser::keysCount() const {
    return _ini_map.at(_cur_group).size();
}

size_t Tiny::IniParser::groupsCount() const {
    return _ini_map.size();
}

std::string & Tiny::IniParser::operator[](const std::string &key) {
    auto iter = findConf(key);
    if (!iter) {
        _ini_map[_cur_group].emplace_back(key, "");
        return _ini_map[_cur_group].back().second;
    }
    return iter->second;
}

Tiny::IniParserError Tiny::IniParser::parseContext(std::string& buf) {
    strip(buf);
    if (buf.empty()) return IniParserError::Success;
    // If the current context is comment, skip parsing.
    if (buf[0] == ';' || buf[0] == '#') return IniParserError::Success;

    size_t key_pos = buf.find_first_of('=');
    bool group_on = (buf.front() == '[');
    bool key_on = (key_pos != std::string::npos);
    if (group_on && key_on) {
        size_t end_of_group_pos = buf.find_last_of(']');
        if (end_of_group_pos < key_pos) return IniParserError::InvalidFormat;
        return IniParserError::InvalidCharacter;
    }
    if (group_on) {
        auto ed_pos = buf.find_last_of(']');
        if (ed_pos == 0 || ed_pos == std::string::npos) return IniParserError::InvalidFormat;
        auto temp = buf.substr(1, ed_pos - 1);
        if (is_include(temp)) {
            return IniParserError::InvalidCharacter;
        }
        _cur_group = temp;
        if (_ini_map.find(_cur_group) == _ini_map.end()) {
            _ini_map[_cur_group] = {};
        }
    } else if (key_on) {
        if (_cur_group.empty()) {
            _cur_group = "ungrouped";
            if (_ini_map.find(_cur_group) == _ini_map.end()) {
                _ini_map[_cur_group] = {};
            }
        }
        auto key = buf.substr(0, key_pos);
        auto value = buf.substr(key_pos + 1, buf.size() - key_pos);
        strip(key);
        if (key.empty()) return IniParserError::InvalidFormat;
        strip(value);
        if (is_include(key) || is_include(value)) return IniParserError::InvalidCharacter;
        _ini_map[_cur_group].emplace_back(key, value);
    } else {
        return IniParserError::InvalidFormat;
    }
    return IniParserError::Success;
}

Tiny::IniConf* Tiny::IniParser::findConf(const std::string &key) {
    auto& group = _ini_map.at(_cur_group);
    size_t idx = 0;
    for (; idx < group.size(); ++idx) {
        if (group[idx].first == key) {
            return &group[idx];
        }
    }
    return nullptr;
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

