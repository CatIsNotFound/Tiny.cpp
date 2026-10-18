#include "../src/Tiny.hpp"

using namespace Tiny;

int main(int argc, char *argv[]) {
    OS::FileSnapShot file_snap_shot;
    OS::FileSystem::mkFile("./test.txt", "Hello world!");
    OS::Path path("./test.txt");
    file_snap_shot.addPath(path);
    file_snap_shot.setTrigger(Tiny::OS::FileSnapShot::All);
    size_t cnt = 10;
    while (cnt--) {
        auto ok = file_snap_shot.check();
        TUI::Terminal::printLine(ok ? "YES" : "NO");
    }
    return 0;
}


