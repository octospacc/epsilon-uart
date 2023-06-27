#ifndef UART_APP_H
#define UART_APP_H

#include <escher.h>
#include "main_controller.h"

namespace Uart {

class App : public ::App {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    I18n::Message name() override;
    I18n::Message upperName() override;
    const Image * icon() override;
  };
  class Snapshot : public ::App::Snapshot {
  public:
    Snapshot();
    App * unpack(Container * container) override;
    void reset() override;
    Descriptor * descriptor() override;
    bool & usart6();
  private:
    bool m_usart6;
  };
  int numberOfTimers() override;
  Timer * timerAtIndex(int i) override;
private:
  App(Container * container, Snapshot * snapshot);
  MainController m_mainController;
};

}

#endif
