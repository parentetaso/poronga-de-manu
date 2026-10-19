// uart_comm.c
// Capa de comunicacion UART por encima del driver de interrupciones

#include <stdint.h>
#include <stdbool.h>     // true / false
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "uart_comm.h"
#include "../Drivers/uart_driver.h"

#define UART_ID        uart0
#define BAUD_RATE      115200
#define UART_TX_PIN    0
#define UART_RX_PIN    1

#define RX_BUF_LEN     256

static volatile uint8_t rx_buffer[RX_BUF_LEN];
static volatile uint16_t rx_head = 0;
static volatile uint16_t rx_tail = 0;

// Envio un byte usando el driver de bajo nivel (definida antes de su uso)
static void uart_comm_write_byte(uint8_t dato) {
    uart_hw_t *hw = uart_get_hw_instance(UART_ID);
    uart_write_byte_drv(hw, (uint32_t)dato);
}

// ISR: guarda los bytes recibidos en el buffer circular
static void uart_rx_isr(void) {
    uart_hw_t *hw = uart_get_hw_instance(UART_ID);
    while (uart_getc_not_blocking_check(hw)) {
        uint8_t b = (uint8_t)uart_read_byte(hw);
        uint16_t next = (rx_head + 1) % RX_BUF_LEN;
        if (next != rx_tail) {          // si el buffer no esta lleno
            rx_buffer[rx_head] = b;
            rx_head = next;
        }
    }
}

void uart_comm_inicializar(void) {
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    uart_driver_inicializar(UART_ID);   // registro de IRQ del driver

    // Habilito la interrupcion de recepcion
    uart_set_irq_enables(UART_ID, true, false);
    irq_add_shared_handler(UART_IRQ_NUM(UART_ID), uart_rx_isr,
                           PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
}

void uart_comm_enviar(const char *texto) {
    while (*texto) {
        uart_putc(UART_ID, *texto++);
    }
}

void uart_comm_enviar_byte(uint8_t dato) {
    uart_comm_write_byte(dato);
}

int uart_comm_disponible(void) {
    return (rx_head - rx_tail + RX_BUF_LEN) % RX_BUF_LEN;
}

int uart_comm_read_byte(uint8_t *dato) {
    if (rx_tail == rx_head) {
        return 0;                        // sin datos
    }
    *dato = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % RX_BUF_LEN;
    return 1;
}
