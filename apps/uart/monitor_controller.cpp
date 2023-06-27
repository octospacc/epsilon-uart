#include "monitor_controller.h"
#include "../apps_container.h"

#include <stdio.h>

namespace Uart {

MonitorController::MonitorController(Responder * parentResponder) :
  ViewController(parentResponder),
  Timer(1),
  m_monitorView(m_buffer, 100, this)
{
}

View * MonitorController::view() {
  return &m_monitorView;
}

bool MonitorController::fire() {
  char c;
  bool refresh = false;
  while (Ion::Console::readCharNonblocking(&c)) {
    m_monitorView.putChar(c);
    refresh = true;
  }

  return refresh;
}

void MonitorController::didBecomeFirstResponder() {
  app()->setFirstResponder(m_monitorView.textField());
}

Toolbox * MonitorController::toolboxForTextInput(TextInput * textInput) {
  // FIXME
  return nullptr;
}

bool MonitorController::textFieldShouldFinishEditing(TextField * textInput, Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool MonitorController::textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) {
  Ion::Console::writeLine(textField->text());

  // TODO: UI to disable local echo?
  const char * p;
  for (p = textField->text(); *p != 0; ++p) {
    m_monitorView.putChar(*p);
  }
  m_monitorView.putChar('\n');

  m_monitorView.clearInput();

  return true;
}

bool MonitorController::textFieldDidReceiveEvent(TextField*, Ion::Events::Event) {
  return false;
}

MonitorController::MonitorView::MonitorView(char * buffer, size_t bufferSize, TextFieldDelegate * delegate) :
  m_buffer(buffer),
  m_draftBuffer(new char[bufferSize]),
  m_lines(0),
  m_length(0),
  m_statusView(KDText::FontSize::Large, 0, 0,
    // Solarized Dark by Ethan Schoonover
    KDColor::RGB24(0x2aa198),
    KDColor::RGB24(0x073642)
  ),
  m_textView(KDText::FontSize::Large, 0, 0,
    KDColor::RGB24(0x657b83),
    KDColor::RGB24(0x002b36)
  ),
  m_textField(nullptr, buffer, m_draftBuffer, bufferSize, delegate, true, KDText::FontSize::Large, 0.f, 0.5f,
    KDColor::RGB24(0x657b83),
    KDColor::RGB24(0xfdf6e3)
  )
{
}

MonitorController::MonitorView::~MonitorView() {
  delete[] m_draftBuffer;
}

void MonitorController::MonitorView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(bounds(), KDColorBlack);
}

int MonitorController::MonitorView::numberOfSubviews() const {
  return 3;
}

View * MonitorController::MonitorView::subviewAtIndex(int index) {
  switch (index) {
    case 0: return &m_textView;
    case 1: return &m_statusView;
    case 2: return &m_textField;
    default:
      assert(false);
      return nullptr;
  }
}

void MonitorController::MonitorView::putChar(const char c) {
  if (c == '\r' || c == '\n') {
    m_textView.appendText("\n");
    m_lines++;
    if (m_lines > k_numberOfLines) {
      trimLine();
    }
    m_length = 0;
  } else {
    if (m_length >= k_lengthLimit) {
      putChar('\n');
    }
    m_length++;
    const char s[2] = { c, 0 };
    m_textView.appendText(s);
    markRectAsDirty(bounds());
  }
}

void MonitorController::MonitorView::clearInput() {
  m_textField.setText("");
}

TextField * MonitorController::MonitorView::textField() {
  return &m_textField;
}

void MonitorController::MonitorView::layoutSubviews() {
  KDCoordinate textHeight = KDText::charSize(KDText::FontSize::Large).height();
  m_textView.setFrame(KDRect(0, 0, bounds().width(), bounds().height() - 3 * textHeight));
  m_statusView.setFrame(KDRect(0, bounds().bottom() - 3 * textHeight, bounds().width(), textHeight));
  m_textField.setFrame(KDRect(0, bounds().bottom() - 2 * textHeight, bounds().width(), 2 * textHeight));
}

void MonitorController::MonitorView::trimLine() {
  const char * text = m_textView.text();
  const char * p;
  for (p = text; *p != 0; ++p) {
    if (*p == '\r' || *p == '\n') {
      break;
    }
  }
  if (*p != 0) {
    char trimmed[256];
    strlcpy(trimmed, p + 1, 256);
    m_textView.setText(trimmed);
    m_lines--;
  }
}

}