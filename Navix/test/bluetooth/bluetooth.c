#include "bluetooth.h"
#include "hardware/uart.h"

void bt_init() {
    // Inicializar el puerto serial 1 a 9600 baudios
    uart_init(UART_BT, BAUD_BT);
    
    // Configurar los pines de la Pico para que actúen como UART
    gpio_set_function(PIN_BT_TX, GPIO_FUNC_UART);
    gpio_set_function(PIN_BT_RX, GPIO_FUNC_UART);
}