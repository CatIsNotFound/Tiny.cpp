
#include "../src/Tiny.hpp"
using namespace Tiny;
using namespace TUI;


int main(int argc, char *argv[]) {
    Application app;
    Label label_1("l1", {2, 10}, {8, 3});
    Label label_2("l2", {2, 20}, {8, 3});
    Label label_3("l3", {2, 30}, {8, 3});
    Label label_4("l4", {6, 10}, {8, 3});
    Label label_5("l5", {6, 20}, {8, 3});
    Label label_6("l6", {6, 30}, {8, 3});
    Label label_7("l7", {10, 10}, {8, 3});
    Label label_8("l8", {10, 20}, {8, 3});
    Label label_9("l9", {10, 30}, {8, 3});
    label_1.setFocus(true);
    label_2.setFocus(true);
    label_3.setFocus(true);
    label_4.setFocus(true);
    label_5.setFocus(true);
    label_6.setFocus(true);
    label_7.setFocus(true);
    label_8.setFocus(true);
    label_9.setFocus(true);
    label_1.setAlignment(Alignment::LeftTop);
    label_2.setAlignment(Alignment::CenterTop);
    label_3.setAlignment(Alignment::RightTop);
    label_4.setAlignment(Alignment::Left);
    label_5.setAlignment(Alignment::Center);
    label_6.setAlignment(Alignment::Right);
    label_7.setAlignment(Alignment::LeftBottom);
    label_8.setAlignment(Alignment::CenterBottom);
    label_9.setAlignment(Alignment::RightBottom);

    return app.run();
}


