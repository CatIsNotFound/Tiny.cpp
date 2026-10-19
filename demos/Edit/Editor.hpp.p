

#ifndef TINY_CPP_DEMO_EDITOR_HPP
#define TINY_CPP_DEMO_EDITOR_HPP
#include "../src/Tiny.hpp"
using namespace Tiny;

class Editor : public TUI::AbstractWidget {
public:
    explicit Editor(const std::string& name, Object* parent = nullptr)
            : TUI::AbstractWidget(name, typeid(Editor), parent) {}
    virtual ~Editor() = default;

    bool load(const std::string& text, size_t length = 0);
    void moveCursor(const TUI::Position& pos);
    void moveCursor(uint32_t row, uint32_t col);
    void moveRows(int32_t row = 1);
    void moveCols(int32_t col = 1);
    void addText(const std::string& text);
    void insertLine(uint32_t row, const std::string& text);
    bool popChar();
    void removeCurrentLine();
    bool removeLines(uint32_t offset, uint32_t count = 1);

    const TUI::Position& cursorPosition() const;
    const std::string& lineOfText(uint32_t row) const;

protected:
    void renderEvent(TUI::Renderer& renderer) override;
    void resizeEvent(uint32_t width, uint32_t height) override;
    void moveEvent(uint32_t x, uint32_t y) override;
    void keyEvent(TUI::KeyEvent keyboard) override;
    void mouseEvent(TUI::MouseEvent mouse) override;
    void focusEvent(bool focus) override;
    void enableEvent(bool enable) override;
    void clickedEvent() override;
    virtual void textAddedEvent();
    virtual void textRemovedEvent();
private:
    std::vector<std::string> _buffers{1, ""};
    uint32_t _s_col{};
    uint32_t _s_row{};
    TUI::Position _cur_pos{};
    bool _auto_line_break{};
    bool _show_line_no{};
    bool _show_line_highlight{};
    bool _need_render{};
};

struct UI {
    UI() {
        _editor.reset(new Editor("editor"));
        _label.reset(new TUI::Label("Editor", {0, 0}));
    }

    void setup() {
        _editor->move(0, 1);
        _editor->resize(TUI::Terminal::screenSize());
    }

    std::unique_ptr<Editor> _editor;
    std::unique_ptr<TUI::Label> _label;
};


#endif //TINY_EDITOR_HPP
