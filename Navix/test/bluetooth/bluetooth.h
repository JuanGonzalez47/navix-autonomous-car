#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "pico/stdlib.h"

// --- Configuración UART1 para HC-05 ---
#define UART_BT uart1
#define BAUD_BT 9600 // Velocidad de fábrica por defecto del HC-05
#define PIN_BT_TX 8  // TX de la Pico (Va al RX del HC-05)
#define PIN_BT_RX 9  // RX de la Pico (Va al TX del HC-05)

void bt_init(void);

#endif