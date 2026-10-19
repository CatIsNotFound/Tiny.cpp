#include "Editor.hpp"
using namespace TUI;

REG_META_EVENT(EditorEvent, Editor);

int main(int argc, char *argv[]) {
    CommandParser cmd_parser(argc, argv);
    cmd_parser.addCommand("file", "f", "Specified the file to open it.", true, {}, false, true);
    cmd_parser.addLastCommand("help", "h?", "Display the help message.");
    cmd_parser.addLastCommand("version", "v", "Display version information.");
    int n{-1};
    auto err = cmd_parser.exec(nullptr, &n);
    if (err != CommandParser::ParseError::NoError) {
        if (n > 0) {
            Terminal::printError("Edit: Unknown argument: \"{}\"\r\n", argv[n]);
        } else {
            Terminal::printError("Edit: Failed to execute command! Try to add \"--help\" for help.\r\n");
        }
        return 1;
    }

    auto exec_list = cmd_parser.execCommandList();
    for (auto& e : exec_list) {
        if (e.option_name == "help") {
            Terminal::printLine(cmd_parser.generateHelpInfo(Terminal::screenSize().width));
            Terminal::printFormat("EXAMPLE:\r\n    - To open 'my_file.txt' file, execute: `Edit my_file.txt`.\r\n`");
            return 0;
        }
        if (e.option_name == "version") {
            Terminal::printLine("Edit version 0.1");
            return 0;
        }
        if (e.option_name == "file") {
            ///
        }
    }

    Application app;
    app.setRefreshEnabled(true);
    UI ui;
    ui.setup();
    auto st = DT::currentTimestamps();
    EventBus::self().subscribe<Application>([&ui, &st](const AbstractEvent& ev) {
        auto ts = DT::currentTimestamps();
        ui._label->setText(DT::formatStdTime(ts - st, true));
    });

    ui._editor->addText("Hello world!");
    ui._editor->insertLine(1, "Here is my world!");

    return app.run();
}
