
#include "../src/Tiny.hpp"
using namespace Tiny;
using namespace TUI;

class MyListView : public ListView {
public:
    MyListView(const std::string& name, const Position& pos, const Size& size) : ListView(name, pos, size) {}
    void setLineEdit(LineEdit* line_edit) {
        _line_edit = line_edit;
    }
    void setButton(Button* btn) {
        _btn = btn;
    }
protected:
    void indexChangedEvent() override {
        ListView::indexChangedEvent();
        if (_line_edit) _line_edit->setText(itemAt(currentIndex()));
        if (_btn) _btn->setText(Terminal::formatString("[Index: {}]", currentIndex()));
    }
private:
    LineEdit* _line_edit{};
    Button* _btn{};
};

int main(int argc, char *argv[]) {
    if (strcmp(OS::Name, "windows") == 0) OS::exec("cmd.exe /c chcp 65001");
    Application app;
    app.setEnabledExitByKey(false);
    Label title("[List view Demo]", {0, 0});
    LineEdit line_edit("line_edit", {1, 0}, 30);
    Button test_btn("test_btn", {1, 32}, {10, 3});
    test_btn.setAlignment(Alignment::Right);
    test_btn.setText("Last: ");
    line_edit.setPlaceHolderText("Click one item or edit text...");
    line_edit.setMaximumLength(20);
    MyListView list_view("list_view", {2, 0}, {15, 6});
    list_view.setLineEdit(&line_edit);
    list_view.setButton(&test_btn);
    Button btn_add("add", {3, 17});
    Button btn_insert("insert", {4, 17});
    Button btn_rm("remove", {5, 17});
    Button btn_clr("clear", {6, 17});
    Button btn_update("update", {7, 17});
    Button btn_quit("quit", {8, 17});
    btn_add.setText("[   add   ]");
    btn_insert.setText("[  insert ]");
    btn_rm. setText("[  remove ]");
    btn_clr.setText("[  clear  ]");
    btn_update.setText("[  update ]");
    btn_add.setEvent([&line_edit, &list_view] (const Button&) {
        if (line_edit.text().empty()) return;
        list_view.appendItem(line_edit.text());
        line_edit.clear();
    });
    btn_insert.setEvent([&line_edit, &list_view] (const Button&) {
        if (line_edit.text().empty()) return;
        list_view.insertItem(list_view.currentIndex(), line_edit.text());
        line_edit.clear();
    });
    btn_rm.setEvent([&list_view](const Button &) {
        list_view.removeItems(list_view.currentIndex());
    });
    btn_clr.setEvent([&list_view] (const Button&) {
        list_view.clear();
    });
    btn_update.setEvent([&line_edit, &list_view] (const Button&) {
        list_view.setItem(list_view.currentIndex(), line_edit.text());
    });
    btn_add.setDefaultKeys(KEY_CTRL_A);
    btn_rm.setDefaultKeys(KEY_BK, KEY_DEL);
    btn_clr.setDefaultKeys(KEY_SPECIAL, KEY_NONE, SP_KEY_DELETE);
    btn_update.setDefaultKeys(KEY_CTRL_U);
    btn_quit.setDefaultKeys(KEY_CTRL_C, KEY_ESC);
    btn_quit.setText("[   quit  ]");
    btn_quit.setEvent([&app](Button&) {
        app.exit();
    });
    CurBlock cur("cur");
    
    return app.run();
}


