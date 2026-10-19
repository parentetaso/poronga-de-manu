// uart_driver.h
// Driver de bajo nivel para UART0/UART1 con interrupciones (RP2040)

#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include <stdint.h>                     // uint32_t, uint8_t
#include <stdbool.h>                    // bool (para uart_driver_irq_enable)
#include "hardware/structs/uart.h"      // uart_hw_t
#include "hardware/uart.h"              // uart_inst_t / uart_id helpers

#define UART_COUNT      2               // uart0 y uart1 en RP2040

typedef enum {
    UART_0 = 0,
    UART_1 = 1
} uart_sel_t;

// Numero de IRQ correspondiente a cada UART
#define UART0_IRQ_NUM   UART0_IRQ
#define UART1_IRQ_NUM   UART1_IRQ

#define UART_IRQ_NUM(id) (((id) == uart1) ? UART1_IRQ_NUM : UART0_IRQ_NUM)

// Accede a la estructura de hardware de una UART
uart_hw_t *uart_get_hw_instance(uart_inst_t *uart);

// Inicializa el driver (relojes, resets e interrupcion)
void uart_driver_inicializar(uart_inst_t *uart);

// Habilita/deshabilita las interrupciones de TX/RX
void uart_driver_irq_enable(uart_inst_t *uart, bool rx, bool tx);

// Lee un byte de la UART (sin bloqueo; solo valido si hay datos)
uint32_t uart_read_byte(uart_hw_t *hw);

// Escribe un byte en la UART (espera a que el FIFO este libre)
void uart_write_byte_drv(uart_hw_t *hw, uint32_t dato);

// Comprueba si hay datos disponibles para leer
bool uart_getc_not_blocking_check(uart_hw_t *hw);

// ISR comun para ambas UARTs (la usan uart0_isr / uart1_isr)
void uart_common_isr(uart_sel_t cual);

#endif // UART_DRIVER_H
