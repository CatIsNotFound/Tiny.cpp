
#include "../src/Tiny.hpp"

using namespace Tiny;
using namespace OS;
using term = TUI::Terminal;

int main(int argc, char *argv[]) {
    FileSystem::mkFile("./my_file.txt", "Hello, world!\r\n");
    Path my_file("./my_file.txt");

    auto access_time = DT::DateTime::formatString("yyyy/MM/dd HH:mm:ss.SSS", {my_file.lastAccessTime()});
    auto modify_time = DT::DateTime::formatString("yyyy/MM/dd HH:mm:ss.SSS", {my_file.lastWriteTime()});
    auto create_time = DT::DateTime::formatString("yyyy/MM/dd HH:mm:ss.SSS", {my_file.lastCreateTime()});

    std::string perms(9, '-');
    auto user_perm = my_file.userPermission();
    auto group_perm = my_file.groupPermission();
    auto other_perm = my_file.otherPermission();
    perms[0] = user_perm & P_Read ? 'r' : '-';
    perms[3] = group_perm & P_Read ? 'r' : '-';
    perms[6] = other_perm & P_Read ? 'r' : '-';

    perms[1] = user_perm & P_Write ? 'w' : '-';
    perms[4] = group_perm & P_Write ? 'w' : '-';
    perms[7] = other_perm & P_Write ? 'w' : '-';

    perms[2] = user_perm & P_Execute ? 'x' : '-';
    perms[5] = group_perm & P_Execute ? 'x' : '-';
    perms[8] = other_perm & P_Execute ? 'x' : '-';

    term::printLine(my_file.path());
    term::printFormat("{}: {}\r\n", "Size", my_file.fileSize());
    term::printFormat("{}: {}\r\n", "Permissions", perms);
    term::printFormat("{}: {}\r\n", "Last access", access_time);
    term::printFormat("{}: {}\r\n", "Last modify", modify_time);
    term::printFormat("{}: {}\r\n", "Last create", create_time);

    FileSystem::rmFile("./my_file.txt");
    return 0;
}

