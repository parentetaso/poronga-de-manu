// I2C_lcd.h
#ifndef I2C_LCD_H
#define I2C_LCD_H

#include <stdint.h>   // uint8_t

void lcd_inicializar(void);
void lcd_limpiar(void);
void lcd_comando(uint8_t cmd);
void lcd_dato(uint8_t dat);
void lcd_send_nibble(uint8_t nibble, uint8_t rs);
void lcd_pos_cursor(uint8_t fila, uint8_t col);
void lcd_imprimir(const char *texto);
void lcd_imprimir_num(int numero);

#endif // I2C_LCD_H
