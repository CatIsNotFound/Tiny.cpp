#include "../src/OS/File.hpp"
#include "../src/TUI/Terminal.hpp"
using namespace Tiny::OS;

int main() {
    using ter = Tiny::TUI::Terminal;
    std::vector<std::string> temp_lines;
    File file("./assets/File/test.txt", ReadOnly);
    if (file.isOpen()) {
        while (!file.isEOF()) {
            temp_lines.push_back(file.readLine());
            ter::print(temp_lines.back());
        }
        ter::printFormat("\r\nRead {} byte(s) from file \"{}\".\r\n", file.fileSize(), file.fileName());
        file.close();
    }
    file.setPath("./test.txt");
    if (file.open(WriteOnly | Append)) {
        for (auto & line : temp_lines) {
            file.write(line);
        }
        file.close();
        ter::printFormat("\r\nWrite contexts to file \"{}\"\r\n", file.fileName());
    } else {
        ter::printFormat("\r\nFailed to open file \"{}\".\r\n", file.fileName());
    }
    return 0;
}