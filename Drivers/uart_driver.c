// uart_driver.c
// Driver de bajo nivel para UART0/UART1 con interrupciones (RP2040)

#include <stdint.h>                      // uint32_t
#include <stdbool.h>                     // bool, true/false
#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hardware/resets.h"
#include "hardware/structs/uart.h"       // uart_hw_t, uart0_hw, uart1_hw
#include "hardware/structs/resets.h"     // resets_hw y RESETS_RESET_UARTx_BITS
#include "hardware/uart.h"
#include "uart_driver.h"

// Handlers de interrupcion por UART (se registran mas abajo)
void uart0_isr_handler(void);
void uart1_isr_handler(void);

// Tablas de acceso por indice (uart0 / uart1)
static const uint32_t uart_irqs[UART_COUNT] = {UART0_IRQ, UART1_IRQ};
static const uint32_t uart_resets[UART_COUNT] = {RESETS_RESET_UART0_BITS, RESETS_RESET_UART1_BITS};

// En el SDK real uart0_hw/uart1_hw son constantes; los obtengo via la API del SDK
uart_hw_t *uart_get_hw_instance(uart_inst_t *uart) {
    if (uart == uart1) {
        return uart1_hw;
    }
    return uart0_hw;
}

static uart_hw_t *uart_hw_sel(uart_sel_t cual) {
    return uart_get_hw_instance(cual == UART_1 ? uart1 : uart0);
}

void uart_driver_inicializar(uart_inst_t *uart) {
    uart_sel_t cual = (uart == uart1) ? UART_1 : UART_0;

    // Aseguro que el bloque no este en reset y habilito su reloj
    reset_clear_num(pnum_uart(cual));
    reset_unclear_num(pnum_uart(cual));
    (void)uart_resets;   // tabla disponible para manejo manual de resets

    // Registro los handlers de interrupcion de cada UART
    irq_set_exclusive_handler((IRQn_Type)uart_irqs[cual],
                              cual == UART_0 ? uart0_isr_handler : uart1_isr_handler);
    irq_set_enabled((IRQn_Type)uart_irqs[cual], true);
}

// ISR propias: solo derivan a la ISR comun pasando la UART correspondiente
void uart0_isr_handler(void) {
    uart_common_isr(UART_0);
}

void uart1_isr_handler(void) {
    uart_common_isr(UART_1);
}

void uart_common_isr(uart_sel_t cual) {
    uart_hw_t *hw = uart_hw_sel(cual);

    // Habilito la interrupcion de recepcion en el registro IMSC (bit RXIM, pos. 4)
    hw->imsc |= 1u << UART_UARTIMSC_RXIM_LSB;

    // Lectura mientras haya datos disponibles en el FIFO de RX
    while (!(hw->fr & UART_FR_RXFE_BITS)) {
        volatile uint32_t dato = hw->dr;
        (void)dato;   // aqui se entregaria al buffer del nivel superior
    }

    // Limpio banderas de error (overrun / framing / parity) si las hubiera
    if (hw->rsr & (UART_RSR_OE_ERROR_BITS | UART_RSR_BE_ERROR_BITS | UART_RSR_PE_ERROR_BITS)) {
        hw->rsr = 0;
    }
}

uint32_t uart_read_byte(uart_hw_t *hw) {
    return hw->dr;
}

void uart_write_byte_drv(uart_hw_t *hw, uint32_t dato) {
    while (hw->fr & UART_FR_TXFF_BITS) {
        // espero a que el FIFO de TX tenga espacio
    }
    hw->dr = dato;
}

bool uart_getc_not_blocking_check(uart_hw_t *hw) {
    return !(hw->fr & UART_FR_RXFE_BITS);
}
