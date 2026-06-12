#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "bluetooth.h"

// Definir el pin del LED integrado de la Raspberry Pi Pico
#define LED_PIN PICO_DEFAULT_LED_PIN

int main()
{
    stdio_init_all();

    // Inicializar el LED integrado
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0); // Asegurarse de que empiece apagado

    sleep_ms(3000);

    printf("--- PRUEBA UNITARIA: BLUETOOTH HC-05 (UART1) ---\n");

    bt_init();
    printf("[Sistema] HC-05 Inicializado. Esperando conexión desde el celular...\n");

    uint32_t ultimo_latido = to_ms_since_boot(get_absolute_time());

    // Variables para el control no bloqueante del LED
    uint32_t tiempo_apagar_led = 0;
    bool led_encendido = false;

    while (true)
    {

        // 1. LEER DEL CELULAR: Si entra un mensaje por Bluetooth, titila el LED
        if (uart_is_readable(UART_BT))
        {
            char c = uart_getc(UART_BT);
            putchar(c); // Imprime letra por letra en tu monitor serial

            // Encender el LED
            gpio_put(LED_PIN, 1);
            led_encendido = true;

            // Programar el apagado para 50 milisegundos en el futuro (titileo rápido)
            tiempo_apagar_led = to_ms_since_boot(get_absolute_time()) + 50;
        }

        // --- LÓGICA DE APAGADO DEL LED ---
        // Si el LED está prendido y ya pasaron los 50ms, lo apagamos sin detener el ciclo
        if (led_encendido && to_ms_since_boot(get_absolute_time()) > tiempo_apagar_led)
        {
            gpio_put(LED_PIN, 0);
            led_encendido = false;
        }

        // 2. LEER DE LA COMPUTADORA: Si escribes algo en el monitor serial, se lo enviamos al celular
        int char_usb = getchar_timeout_us(0);
        if (char_usb != PICO_ERROR_TIMEOUT)
        {
            uart_putc(UART_BT, (char)char_usb);
        }

        // 3. HEARTBEAT: Cada 5 segundos enviamos un saludo automático
        if (to_ms_since_boot(get_absolute_time()) - ultimo_latido > 5000)
        {
            char *msg = "[Pico] ¡Hola Celular! El robot sigue vivo.\r\n";
            while (*msg)
            {
                uart_putc(UART_BT, *msg++);
            }
            ultimo_latido = to_ms_since_boot(get_absolute_time());
        }
    }
}