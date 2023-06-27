#include "app.h"
#include "../apps_container.h"
#include "uart_icon.h"

using namespace Poincare;

namespace Uart {

I18n::Message App::Descriptor::name() {
  return I18n::Message::UartApp;
}

I18n::Message App::Descriptor::upperName() {
  return I18n::Message::UartAppCapital;
}

const Image * App::Descriptor::icon() {
  return ImageStore::UartIcon;
}

App::Snapshot::Snapshot() :
  m_usart6(false)
{
}

App * App::Snapshot::unpack(Container * container) {
  return new App(container, this);
}

void App::Snapshot::reset() {
}

bool & App::Snapshot::usart6() {
  return m_usart6;
}

App::Descriptor * App::Snapshot::descriptor() {
  static Descriptor descriptor;
  return &descriptor;
}

App::App(Container * container, Snapshot * snapshot) :
  ::App(container, snapshot, &m_mainController),
  m_mainController(this, snapshot->usart6())
{
}

int App::numberOfTimers() {
  return 1;
}

Timer * App::timerAtIndex(int i) {
  assert(i == 0);
  return m_mainController.getRxTimer();
}

}
