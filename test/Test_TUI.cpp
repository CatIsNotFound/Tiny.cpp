
#include "../src/Tiny.hpp"
using namespace Tiny;
using namespace TUI;

class MyListView : public ListView {
public:
    MyListView(const std::string& name, const Position& pos, const Size& size) : ListView(name, pos, size) {}
    void setLineEdit(LineEdit* line_edit) {
        _line_edit = line_edit;
    }
protected:
    void indexChangedEvent() override {
        ListView::indexChangedEvent();
        if (_line_edit) _line_edit->setText(itemAt(currentIndex()));
    }
private:
    LineEdit* _line_edit{};
};

int main(int argc, char *argv[]) {
    if (strcmp(OS::Name, "windows") == 0) OS::exec("cmd.exe /c chcp 65001");
    Application app;
    app.setRefreshEnabled(false);
    Label title("[List view Demo]", {0, 0});
    LineEdit line_edit("line_edit", {1, 0}, 30);
    line_edit.setPlaceHolderText("Click one item or edit text...");
    line_edit.setMaximumLength(20);
    MyListView list_view("list_view", {2, 0}, {15, 6});
    list_view.setLineEdit(&line_edit);
    Button btn_add("add", {3, 17});
    Button btn_insert("insert", {4, 17});
    Button btn_rm("remove", {5, 17});
    Button btn_clr("clear", {6, 17});
    Button btn_update("update", {7, 17});
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
    btn_rm.setEvent([&list_view] (const Button&) {
        list_view.removeItems(list_view.currentIndex());
    });
    btn_clr.setEvent([&list_view] (const Button&) {
        list_view.clear();
    });
    btn_update.setEvent([&line_edit, &list_view] (const Button&) {
        list_view.setItem(list_view.currentIndex(), line_edit.text());
    });

    CurBlock cur("cur");
    
    return app.run();
}


