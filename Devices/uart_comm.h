// uart_comm.h
#ifndef UART_COMM_H
#define UART_COMM_H

#include <stdint.h>   // uint8_t

void uart_comm_inicializar(void);
void uart_comm_enviar(const char *texto);
void uart_comm_enviar_byte(uint8_t dato);
int  uart_comm_disponible(void);            // bytes disponibles para leer
int  uart_comm_read_byte(uint8_t *dato);    // 1 si habia datos, 0 si no

#endif // UART_COMM_H
