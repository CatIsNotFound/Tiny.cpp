
#include "../src/Tiny.hpp"
using namespace Tiny;
using namespace TUI;

int main(int argc, char *argv[]) {
    Application app("Test");
    Label label("label_1", {1, 1}, &app);
    label.move(2, 4);
    auto ptr = label.parent()->objectName();
    label.setText("Hello world!");
    return app.run();
}


