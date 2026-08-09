#include "../src/Tiny.hpp"

using namespace Tiny;
using namespace TUI;

std::vector<std::string> split(const std::string& str, size_t per_size) {
    std::vector<std::string> result;
    size_t sz = str.size() / per_size;
    if (str.size() % per_size != 0) sz += 1;
    result.reserve(sz);
    for (size_t i = 0; i < sz; ++i) {
        result.push_back(str.substr(per_size * i, per_size));
    }
    return result;
}


void print_help(CommandParser& cmd_parser) {
    auto scr = Terminal::screenSize();
    Terminal::printFormat("Usage: du [-f] <Path> [-h | -a]\r\n\r\nOPTIONS:\r\n");
    for (auto& cmd : cmd_parser) {
        Terminal::printFormat("{:>8c}--{:<10s}, ", ' ', cmd.first);
        int idx = 0;
        std::string short_arg;
        for (auto& opt : cmd.second.short_options) {
            short_arg += Terminal::formatString("-{}", opt);
            if (++idx < cmd.second.short_options.size()) {
                short_arg += " | ";
            }
        }

        Terminal::printFormat("{:<16s} ", short_arg.c_str());
        auto pos = Terminal::cursorPosition();
        size_t dis_size = scr.width - pos.column;
        if (dis_size < cmd.second.description.size()) {
            auto texts = split(cmd.second.description, dis_size);
            for (size_t i = 0; i < texts.size(); ++i) {
                Terminal::printLine(texts[i]);
                if (i != texts.size() - 1) {
                    Terminal::moveCursor(pos.row + i, pos.column);
                }
            }
        } else {
            Terminal::printLine(cmd.second.description);
        }
    }
}

void printResult(size_t sz, const OS::Path& path, bool humanity) {
    OS::DataUnit unit{OS::DataUnit::B};
    if (humanity) {
        auto lf = OS::autoConvertDataSize(sz, unit);
        Terminal::printFormat("{:8.2f}{:<3s} {}\r\n", lf, OS::dataUnitName(unit), path.path());
    } else {
        Terminal::printFormat("{:10s} {}\r\n", sz, path.path());
    }
}

int calcSizeOfPath(const std::string& path, bool humanity, bool shown_all) {
    OS::Path my_path(path);
    if (!my_path.isValid()) {
        Terminal::perror() << TStyle::bg(Color::Red) << "du: The specified path \""
                           << TStyle::bold() << my_path.path() << TStyle::bold(false) << "\" does not exist!"
                           << TStyle::reset() << "\r\n";
        return 2;
    }
    if (my_path.isFile()) {
        printResult(my_path.fileSize(), my_path, humanity);
    } else {
        size_t sz = 0;
        auto paths = OS::FileSystem::listPathEx(my_path, 0,
            [&sz, &humanity, &shown_all](const OS::Path& path, bool&) {
            if (!path.isDirectory()) {
                sz += path.fileSize();
                if (shown_all) printResult(path.fileSize(), path, humanity);
            }
            return true;
        });
        printResult(sz, my_path, humanity);
    }

    return 0;
}

int main(int argc, char* argv[]) {
    CommandParser parser(argc, argv);
    parser.addCommand("file", "f", "Specify a file or directory", true, {}, true, true);
    parser.addCommand("humanity", "h", "Readability Enhancement.");
    parser.addCommand("all", "a", "When specifying a directory, output all files and directories in the directory together.");
    parser.addLastCommand("help", "?", "Display this help.");
    std::vector<std::string> missing;
    int e_pos;
    auto err = parser.exec(nullptr, &e_pos, &missing);
    if (err != CommandParser::ParseError::NoError) {
        Terminal::perror() << TStyle::fg(Color::Red) << "du: " << CommandParser::getParseErrorName(err);
        switch (err) {
            case CommandParser::ParseError::MissingArgument:
            case CommandParser::ParseError::MissingDefaultCommand:
                Terminal::perror() << ": \"" << missing.at(0) << "\".";
                break;
            default:
                Terminal::perror() << ": \"" << argv[e_pos] << "\".";
                break;
        }
        Terminal::perror() << "\r\nPlease type '--help' to display this help!" << TStyle::reset() << "\r\n" ;
        return 1;
    }
    auto exec_list = parser.execCommandList();
    bool humanity = false;
    bool shown_all = false;
    std::string path = "";
    for (auto& e : exec_list) {
        if (e.option_name == "humanity") {
            humanity = true;
        } else if (e.option_name == "all") {
            shown_all = true;
        } else if (e.option_name == "file") {
            path = e.value;
        } else if (e.option_name == "help") {
            print_help(parser);
            return 0;
        }
    }
    return calcSizeOfPath(path, humanity, shown_all);
}
