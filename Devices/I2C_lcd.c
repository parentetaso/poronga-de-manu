// I2C_lcd.c
// Driver para LCD 16x2 con expander I2C PCF8574 (Pico SDK / RP2040)

#include <stdint.h>          // uint8_t, uint32_t
#include <stdbool.h>         // bool (la usa hardware/i2c.h)
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "I2C_lcd.h"

#define I2C_PORT        i2c1
#define I2C_SDA_PIN     10
#define I2C_SCL_PIN     11
#define I2C_SPEED       400000

#define LCD_ADDR        0x27   // direccion del PCF8574 (ajustar segun jumper A0-A2)

// Bits del PCF8574 conectados al LCD (mapa tipico)
#define LCD_RS          0x01
#define LCD_RW          0x02
#define LCD_EN          0x04
#define LCD_BACKLIGHT   0x08
#define LCD_D4          0x10
#define LCD_D5          0x20
#define LCD_D6          0x40
#define LCD_D7          0x80

static uint8_t estado = LCD_BACKLIGHT;

static void lcd_delay_us(uint32_t us) {
    busy_wait_us(us);
}

// Enviamos un byte completo (4 bits altos + 4 bits bajos) al LCD
static void lcd_escribir_byte(uint8_t dat, uint8_t rs) {
    uint8_t hi = dat & 0xF0;
    uint8_t lo = (dat << 4) & 0xF0;

    lcd_send_nibble(hi, rs);
    lcd_send_nibble(lo, rs);
}

void lcd_send_nibble(uint8_t nibble, uint8_t rs) {
    uint8_t out = (nibble & 0xF0) | (rs ? LCD_RS : 0) | LCD_BACKLIGHT;

    // Pulso de enable
    i2c_write_blocking(I2C_PORT, LCD_ADDR, &(uint8_t){out | LCD_EN}, 1, false);
    lcd_delay_us(1);
    i2c_write_blocking(I2C_PORT, LCD_ADDR, &out, 1, false);
    lcd_delay_us(100);
}

void lcd_comando(uint8_t cmd) {
    lcd_escribir_byte(cmd, 0);
}

void lcd_dato(uint8_t dat) {
    lcd_escribir_byte(dat, 1);
}

void lcd_inicializar(void) {
    // Configuro I2C
    i2c_init(I2C_PORT, I2C_SPEED * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    lcd_delay_us(50000);      // espero >40 ms tras el encendido

    // Secuencia de inicializacion en modo 4 bits (HD44780)
    lcd_send_nibble(0x30, 0);
    lcd_delay_us(5000);
    lcd_send_nibble(0x30, 0);
    lcd_delay_us(150);
    lcd_send_nibble(0x30, 0);
    lcd_send_nibble(0x20, 0);

    lcd_comando(0x28);        // 2 lineas, 4 bits
    lcd_comando(0x0C);        // display on, cursor off
    lcd_comando(0x06);        // incremento del cursor
    lcd_comando(0x01);        // limpiar pantalla
    lcd_delay_us(2000);
}

void lcd_limpiar(void) {
    lcd_comando(0x01);
    lcd_delay_us(2000);
}

void lcd_pos_cursor(uint8_t fila, uint8_t col) {
    uint8_t addr = 0x80 + (fila == 0 ? 0x00 : 0x40) + col;
    lcd_comando(addr);
}

void lcd_imprimir(const char *texto) {
    while (*texto) {
        lcd_dato((uint8_t)*texto++);
    }
}

void lcd_imprimir_num(int numero) {
    char buf[12];
    int i = 0;
    if (numero == 0) {
        lcd_dato('0');
        return;
    }
    while (numero != 0) {
        buf[i++] = '0' + (numero % 10);
        numero /= 10;
    }
    while (i > 0) {
        lcd_dato((uint8_t)buf[--i]);
    }
}
