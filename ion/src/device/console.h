#ifndef ION_DEVICE_CONSOLE_H
#define ION_DEVICE_CONSOLE_H

#include <ion/console.h>
#include "regs/regs.h"

namespace Ion {
namespace Console {
namespace Device {

/*  Pin | Role              | Mode
 * -----+-------------------+--------------------
 * PC11 | UART3 RX          | Alternate Function
 *  PD8 | UART3 TX          | Alternate Function
 */

void init(bool usart6 = false);
void shutdown(bool usart6 = false);
bool peerConnected();

constexpr USART UARTPort = USART(3);
constexpr static GPIOPin RxPin = GPIOPin(GPIOC, 11);
constexpr static GPIOPin TxPin = GPIOPin(GPIOD, 8);
constexpr static GPIOPin Pins[] = { RxPin, TxPin };

constexpr USART UsbUARTPort = USART(6);
constexpr static GPIOPin UsbRxPin = GPIOPin(GPIOA, 12);
constexpr static GPIOPin UsbTxPin = GPIOPin(GPIOA, 11);
constexpr static GPIOPin UsbPins[] = { UsbRxPin, UsbTxPin };

}
}
}

#endif
