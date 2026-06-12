#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "bluetooth.h"

int main() {
    stdio_init_all();
    
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    sleep_ms(1000);

    printf("--- PRUEBA UNITARIA: BLUETOOTH HC-05 (UART1) ---\n");
    
    bt_init();
    printf("[Sistema] HC-05 Inicializado. Esperando conexión desde el celular...\n");

    uint32_t ultimo_latido = to_ms_since_boot(get_absolute_time());

    while (true) {
        
        // 1. LEER DEL CELULAR: Si entra un mensaje por Bluetooth, lo mostramos en la computadora
        if (uart_is_readable(UART_BT)) {
            char c = uart_getc(UART_BT);
            putchar(c); // Imprime letra por letra en tu monitor serial
        }

        // 2. LEER DE LA COMPUTADORA: Si escribes algo en el monitor serial, se lo enviamos al celular
        int char_usb = getchar_timeout_us(0);
        if (char_usb != PICO_ERROR_TIMEOUT) {
            uart_putc(UART_BT, (char)char_usb);
        }

        // 3. HEARTBEAT: Cada 5 segundos enviamos un saludo automático al celular para saber que estamos vivos
        if (to_ms_since_boot(get_absolute_time()) - ultimo_latido > 5000) {
            uart_puts(UART_BT, "[Pico] ¡Hola Celular! El robot sigue vivo.\r\n");
            ultimo_latido = to_ms_since_boot(get_absolute_time());
        }
    }
}