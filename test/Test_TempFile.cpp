#include "../src/OS/File.hpp"
#include "../src/TUI/Terminal.hpp"
using namespace Tiny::OS;

int main() {
    using ter = Tiny::TUI::Terminal;
    File file("./assets/File/test.txt", ReadOnly);
    if (file.isOpen()) {
        while (!file.isEOF()) {
            ter::print(file.readLine());
        }
        ter::printFormat("\r\nRead {} byte(s) from file \"{}\".\r\n", file.fileSize(), file.fileName());
        file.close();
    }
    return 0;
}