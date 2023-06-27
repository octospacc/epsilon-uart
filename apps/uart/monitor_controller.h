#ifndef APPS_UART_MONITOR_CONTROLLER_H
#define APPS_UART_MONITOR_CONTROLLER_H

#include <escher.h>

namespace Uart {

class MonitorController : public ViewController, public Timer, public TextFieldDelegate {
public:
  MonitorController (Responder * parentResponder);
  View * view() override;
  bool fire() override;
  void didBecomeFirstResponder() override;
  Toolbox * toolboxForTextInput(TextInput * textInput) override;
  bool textFieldShouldFinishEditing(TextField * textInput, Ion::Events::Event event) override;
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(TextField*, Ion::Events::Event) override;
private:
  class MonitorView : public View {
  public:
    MonitorView(char * buffer, size_t bufferSize, TextFieldDelegate * delegate);
    ~MonitorView();
    void drawRect(KDContext * ctx, KDRect rect) const override;
    int numberOfSubviews() const override;
    View * subviewAtIndex(int index) override;

    void putChar(const char c);
    void clearInput();

    TextField * textField();
  private:
    constexpr static int k_numberOfLines = 8;
    constexpr static int k_lengthLimit = 30;
    void layoutSubviews() override;
    void trimLine();
    char * m_buffer;
    char * m_draftBuffer;
    int m_lines;
    int m_length;
    BufferTextView m_statusView;
    BufferTextView m_textView;
    TextField m_textField;
  };
  char m_buffer[100];
  MonitorView m_monitorView;
};

}

#endif