// RASPI2_G6.c
// Programa principal del proyecto (Raspberry Pi Pico / RP2040)

#include <stdio.h>
#include <stdbool.h>     // true / false
#include "pico/stdlib.h"
#include "../Devices/I2C_lcd.h"
#include "../Devices/teclado.h"
#include "../Devices/uart_comm.h"

int main(void) {
    stdio_init_all();          // stdout por USB (pico/stdlib.h)

    bool running = true;

    lcd_inicializar();
    teclado_inicializar();
    uart_comm_inicializar();

    lcd_imprimir("Sistema listo");

    while (running) {
        char tecla = teclado_leer();
        if (tecla != 0) {
            lcd_pos_cursor(1, 0);
            lcd_dato((uint8_t)tecla);
            uart_comm_enviar_byte((uint8_t)tecla);
        }

        uint8_t rx;
        while (uart_comm_read_byte(&rx)) {
            lcd_pos_cursor(0, 0);
            lcd_dato(rx);
        }

        sleep_ms(10);
    }

    return 0;
}
