#include <ion.h>
#include "regs/nvic.h"
#include "console.h"
#include "ring_buffer.h"

extern volatile RingBuffer<char, 1024> usart6_rx_buffer;

/* This file implements a serial console.
 * We use a 115200 8N1 serial port */

namespace Ion {
namespace Console {

static USART getPreferredPort() {
  return RCC.APB2ENR()->getUSART6EN() ? USART(6) : USART(3);
}

char readChar() {
  USART port = getPreferredPort();
  if (port == USART(6) && !usart6_rx_buffer.empty()) {
    // Use USART6 RX buffer
    while (usart6_rx_buffer.empty()) {
    }
    return usart6_rx_buffer.shift();
  }
  while (port.SR()->getRXNE() == 0) {
  }
  return (char)port.DR()->get();
}

void writeChar(char c) {
  USART port = getPreferredPort();
  while (port.SR()->getTXE() == 0) {
  }
  port.DR()->set(c);
}

bool readCharNonblocking(char * dest) {
  USART port = getPreferredPort();
  if (port == USART(6)) {
    // Use usart6_buffer
    if (!usart6_rx_buffer.empty()) {
      *dest = usart6_rx_buffer.shift();
      return true;
    }
    return false;
  }

  if (port.SR()->getRXNE() == 0) {
    return false;
  }
  *dest = (char)port.DR()->get();
  return true;
}

}
}

namespace Ion {
namespace Console {
namespace Device {

void init(bool usart6) {
  USART port = usart6 ? UsbUARTPort : UARTPort;
  const auto & pins = usart6 ? UsbPins : Pins;

  if (usart6) {
    // Enable USART6
    RCC.APB2ENR()->setUSART6EN(true);
  } else {
    // Enable USART3 (default)
    RCC.APB1ENR()->setUSART3EN(true);
  }

  for(const GPIOPin & g : pins) {
    g.group().MODER()->setMode(g.pin(), GPIO::MODER::Mode::AlternateFunction);
    if (usart6) {
      // AF8: PA12 - USART6_RX, PA11 - USART6_TX
      g.group().AFR()->setAlternateFunction(g.pin(), GPIO::AFR::AlternateFunction::AF8);
    } else {
      // AF7: PC11 - USART3_RX, PD8  - USART3_TX
      g.group().AFR()->setAlternateFunction(g.pin(), GPIO::AFR::AlternateFunction::AF7);
    }
  }

  port.CR1()->setUE(true);
  port.CR1()->setTE(true);
  port.CR1()->setRE(true);

  /* We need to set the baud rate of the UART port.
   * This is set relative to the APB1 clock, which runs at 48 MHz.
   *
   * The baud rate is set by the following equation:
   * BaudRate = fAPB1/(16*USARTDIV), where USARTDIV is a divider.
   * In other words, USARDFIV = fAPB1/(16*BaudRate). All frequencies in Hz.
   *
   * In our case, fAPB1 = 48 MHz, so USARTDIV = 26.0416667
   * DIV_MANTISSA = 26
   * DIV_FRAC = 16*0.0416667 = 1
   */
  if (!usart6) {
    port.BRR()->setDIV_MANTISSA(26);
  } else {
    port.CR1()->setRXNEIE(true);
    port.BRR()->setDIV_MANTISSA(52);
    NVIC.NVIC_ISER2()->set(7, 1);
  }
  port.BRR()->setDIV_FRAC(1);
}

void shutdown(bool usart6) {
  if (!usart6) {
    // Take down USART3
    for(const GPIOPin & g : Pins) {
      g.group().MODER()->setMode(g.pin(), GPIO::MODER::Mode::Analog);
      g.group().PUPDR()->setPull(g.pin(), GPIO::PUPDR::Pull::None);
    }
  } else {
    // Restore PA12 and PA11 to USB
    for(const GPIOPin & g : UsbPins) {
      // AF10: PA12 - USB_FS_DP, PA11 - USB_FS_DM
      g.group().MODER()->setMode(g.pin(), GPIO::MODER::Mode::AlternateFunction);
      g.group().AFR()->setAlternateFunction(g.pin(), GPIO::AFR::AlternateFunction::AF10);
    }
    RCC.APB2ENR()->setUSART6EN(false);
  }
}

bool peerConnected() {
  RxPin.group().PUPDR()->setPull(RxPin.pin(), GPIO::PUPDR::Pull::Down);
  RxPin.group().MODER()->setMode(RxPin.pin(), GPIO::MODER::Mode::Input);
  msleep(1);
  bool result = RxPin.group().IDR()->get(RxPin.pin());
  RxPin.group().PUPDR()->setPull(RxPin.pin(), GPIO::PUPDR::Pull::None);
  RxPin.group().MODER()->setMode(RxPin.pin(), GPIO::MODER::Mode::AlternateFunction);
  return false;
}


}
}
}
