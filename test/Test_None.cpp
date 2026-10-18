#include "../src/Tiny.hpp"

using namespace Tiny;

int main(int argc, char *argv[]) {
    OS::FileSnapShot file_snap_shot;
    OS::FileSystem::mkFile("./test.txt", "Hello world!");
    OS::FileSystem::mkDir("./test");
    OS::Path path("./test");
    OS::Path p2("./test.txt");
    file_snap_shot.addPath(path);
    file_snap_shot.addPath(p2);
    file_snap_shot.setTrigger(Tiny::OS::FileSnapShot::Metadata);
    std::vector<uint64_t> res;
    size_t cnt = 10;
    while (cnt--) {
        auto ok = file_snap_shot.check(1, 1000, &res);
        if (ok) {
            for (auto& u : res) {
                if (u == path.uniqueID()) {
                    TUI::Terminal::printLine(path.path());
                } else if (u == p2.uniqueID()) {
                    TUI::Terminal::printLine(p2.path());
                }
            }
            TUI::Terminal::printLine();
        }
    }
    return 0;
}


