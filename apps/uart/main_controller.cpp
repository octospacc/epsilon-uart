#include <ion.h>
#include "main_controller.h"
#include "../i18n.h"

#if HAS_UART
#include <console.h> // From private Ion headers
#endif

namespace Uart {

MainController::MainController(Responder * parentResponder, bool & usart6) :
  ViewController(parentResponder),
  m_selectableTableView(this, this, 0, 1,
    Metric::CommonTopMargin, Metric::CommonRightMargin, Metric::CommonBottomMargin, Metric::CommonLeftMargin,
    this
  ),
  m_monitorController(this),
  m_usart6(usart6)
{
#if HAS_UART
  m_usart6 = RCC.APB2ENR()->getUSART6EN();
#endif
}

bool MainController::handleEvent(Ion::Events::Event event) {
#if HAS_UART
  if (selectedRow() == 0) {
    if (event == Ion::Events::EXE || event == Ion::Events::OK) {
      m_usart6 = !m_usart6;
    } else {
      return false;
    }

    if (m_usart6) {
      Ion::Console::Device::init(true);
    } else {
      Ion::Console::Device::shutdown(true);
    }
    // m_selectableTableView.reloadCellAtLocation(0, 0);
    m_selectableTableView.reloadData();
    return true;
  }
#endif
  if (selectedRow() == 4) {
    if (event == Ion::Events::EXE || event == Ion::Events::OK) {
      app()->displayModalViewController(&m_monitorController, 0.f, 0.f);
      return true;
    }
  }
  return false;
}

void MainController::didBecomeFirstResponder() {
  if (selectedRow() < 0) {
    selectCellAtLocation(0, 0);
  }
  app()->setFirstResponder(&m_selectableTableView);
}

int MainController::numberOfRows() {
  // USART6 Switch/Message, USART3EN Status, USART6EN Status, Pinout, Monitor Chevron
  return 5;
}

KDCoordinate MainController::rowHeight(int j) {
  return Metric::ParameterCellHeight;
}

KDCoordinate MainController::cumulatedHeightFromIndex(int j) {
  return j * rowHeight(0);
}

int MainController::indexFromCumulatedHeight(KDCoordinate offsetY) {
  return offsetY / rowHeight(0);
}

HighlightCell * MainController::reusableCell(int index, int type) {
  if (type == 0) {
    assert(index == 0);
    return &m_cellWithSwitch;
  } else if (type == 1) {
    assert(index < k_numberOfCellsWithBuffer);
    return &m_cellsWithBuffer[index];
  } else if (type == 2) {
    assert(index == 0);
    return &m_cellWithChevron;
  } else {
    assert(false);
    return nullptr;
  }
}

int MainController::reusableCellCount(int type) {
  if (type == 0) {
    return 1;
  } else if (type == 1) {
    return k_numberOfCellsWithBuffer;
  } else if (type == 2) {
    return 1;
  } else {
    assert(false);
    return 0;
  }
}

int MainController::typeAtLocation(int i, int j) {
  // 0 - Switch, 1 - Buffer, 2 - Chevron
  assert(i == 0);
  assert(j < 5);
#if HAS_UART
  if (j == 0) return 0;
#endif
  if (j <= 3) return 1;
  return 2;
}

void MainController::willDisplayCellForIndex(HighlightCell * cell, int index) {
  if (index == 0) {
    // USART6EN Switch
#if HAS_UART
    MessageTableCellWithSwitch * cellWithSwitch = (MessageTableCellWithSwitch *)cell;
    cellWithSwitch->setMessage(I18n::Message::UartOverUsb);
    SwitchView * switchView = (SwitchView *)cellWithSwitch->accessoryView();
    switchView->setState(m_usart6);
#else
    MessageTableCellWithBuffer * cellWithBuffer = static_cast<MessageTableCellWithBuffer *>(cell);
    cellWithBuffer->setMessage(I18n::Message::UartOverUsb);
    cellWithBuffer->setAccessoryText("???");
#endif
  } else if (index < 3) {
    // USART3EN/USART6EN Status
    MessageTableCellWithBuffer * cellWithBuffer = static_cast<MessageTableCellWithBuffer *>(cell);
    if (index == 1) {
      cellWithBuffer->setMessage(I18n::Message::USART3EN);
    } else {
      cellWithBuffer->setMessage(I18n::Message::USART6EN);
    }
#if HAS_UART
    bool enabled = index == 1 ? RCC.APB1ENR()->getUSART3EN() : RCC.APB2ENR()->getUSART6EN();
    if (enabled) {
      cellWithBuffer->setAccessoryText("1");
    } else {
      cellWithBuffer->setAccessoryText("0");
    }
#else
    cellWithBuffer->setAccessoryText("???");
#endif
  } else if (index == 3) {
    // Pinout info
    MessageTableCellWithBuffer * cellWithBuffer = static_cast<MessageTableCellWithBuffer *>(cell);
#if HAS_UART
    cellWithBuffer->setMessage(I18n::Message::UartPinout);
#else
    cellWithBuffer->setMessage(I18n::Message::UartUnavailable);
#endif
  } else if (index == 4) {
    // Monitor entry
    MessageTableCellWithChevron * cellWithChevron = static_cast<MessageTableCellWithChevron *>(cell);
    cellWithChevron->setMessage(I18n::Message::UartMonitor);
  } else {
    assert(false);
  }
}

View * MainController::view() {
  return &m_selectableTableView;
}

Timer * MainController::getRxTimer() {
  return &m_monitorController;
}

}
