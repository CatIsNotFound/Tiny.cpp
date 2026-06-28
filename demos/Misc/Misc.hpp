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

#ifndef TINY_CPP_MISC_MISC_HPP
#define TINY_CPP_MISC_MISC_HPP

#include <string>
#include <vector>
#include <functional>
#include <cstring>
#include <cstdint>

namespace Tiny {
    namespace Misc {
        template <typename T>
        T min(T value1, T value2, const std::function<T(T&, T&)>& compare_func = {}) {
            if (compare_func) return compare_func(value1, value2);
            return value1 < value2 ? value1 : value2;
        }

        template <typename T>
        T max(T value1, T value2, const std::function<T(T&, T&)>& compare_func = {}) {
            if (compare_func) return compare_func(value1, value2);
            return value1 > value2 ? value1 : value2;
        }

        template <typename T>
        T clamp(T value, T min, T max, const std::function<T(T&, T&, T&)>& compare_func = {}) {
            if (compare_func) return compare_func(value, min, max);
            return value < min ? min : value > max ? max : value;
        }

        template <typename T, typename... Args>
        void add(T& result, T& value, Args&&... args) {
            result += value;
            if (sizeof...(args) > 0) {
                add(result, std::forward<Args>(args)...);
            }
        }

        template <typename T, typename... Args>
        void minus(T& result, T& value, Args&&... args) {
            result -= value;
            if (sizeof...(args) > 0) {
                minus(result, std::forward<Args>(args)...);
            }
        }

        template <typename T, typename... Args>
        void times(T& result, T& value, Args&&... args) {
            result *= value;
            if (sizeof...(args) > 0) {
                times(result, std::forward<Args>(args)...);
            }
        }

        template <typename T, typename... Args>
        void divide(T& result, T& value, Args&&... args) {
            if (value != 0) result /= value;
            if (sizeof...(args) > 0) {
                divide(result, std::forward<Args>(args)...);
            }
        }


        inline std::vector<std::string> split(const std::string &s, char delimiter) {
            std::vector<std::string> elems;
            std::string temp;
            for (auto& c : s) {
                if (c == delimiter) {
                    elems.push_back(temp);
                    temp.clear();
                    continue;
                }
                temp += c;
            }
            elems.push_back(temp);
            return elems;
        }

        inline std::vector<std::string> split(const std::string &s, const std::string &delimiter) {
            std::vector<std::string> elems;
            std::string temp;
            size_t prev = 0, pos = 0;
            while (true) {
                pos = s.find(delimiter, prev);
                if (pos == std::string::npos) {
                    elems.emplace_back(s.substr(prev));
                    break;
                }
                elems.emplace_back(s.substr(prev, pos - prev));
                prev = pos + 1;
            }

            return elems;
        }

        inline bool replace(std::string& str, char src, char dest, size_t start_pos = 0) {
            size_t pos = str.find(src, start_pos);
            if (pos == std::string::npos) return false;
            str[pos] = dest;
            return true;
        }

        inline bool replace(std::string& str, const char* src, const char* dest, size_t start_pos = 0) {
            size_t pos = str.find(src, start_pos);
            if (pos == std::string::npos) return false;
            std::string temp = str.substr(0, pos);
            temp += dest;
            temp += str.substr(pos + strlen(src));
            str = temp;
            return true;
        }

        inline bool replaceAll(std::string& str, const char* src, const char* dest, size_t start_pos = 0) {
            size_t pos = str.find(src, start_pos);
            if (pos == std::string::npos) return false;
            std::string temp;
            size_t length = strlen(src);
            do {
                temp += str.substr(start_pos, pos - start_pos);
                temp += dest;
                start_pos = pos + length;
                pos = str.find(src, start_pos);
            } while (pos != std::string::npos);
            str = temp;
            return true;
        }

        inline std::string merge(const std::vector<std::string>& container) {
            std::string result;
            for (auto& c : container) {
                result += c;
            }
            return result;
        }

        inline std::vector<std::string> splitUTF8(const char *data) {
            std::vector<std::string> result;
            if (data == nullptr) return result;
            while (*data) {
                std::string str;
                uint8_t ch = *data;
                if ((ch & 0xE0) == 0xc0) {
                    str += *data++;
                    str += *data++;
                } else if ((ch & 0xF0) == 0xE0) {
                    str += *data++;
                    str += *data++;
                    str += *data++;
                } else if ((ch & 0xF8) == 0xF0) {
                    str += *data++;
                    str += *data++;
                    str += *data++;
                    str += *data++;
                } else {
                    str += *data++;
                }
                result.push_back(str);
            }
            return result;
        }

        inline std::string makeStringOnCenter(const std::string& data, size_t size) {
            std::string result;
            auto utf8_chars = splitUTF8(data.c_str());
            auto width = max<size_t>(utf8_chars.size(), size);
            if (width == utf8_chars.size()) {
                result = data;
                return result;
            }
            size_t half1 = width / 2, half2 = utf8_chars.size() / 2;
            size_t st_pos = 0;
            size_t idx = 0;
            if (half1 > half2) st_pos = half1 - half2;
            result.insert(result.begin(), st_pos, ' ');
            result.insert(result.begin() + st_pos, data.cbegin(), data.cend());
            result.insert(result.end(), st_pos, ' ');
            return result;
        }
    }
}

#endif //TINY_CPP_MISC_MISC_HPP

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