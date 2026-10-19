// teclado.c
// Matriz de teclado 4x4 leida por GPIO (Pico SDK / RP2040)

#include <stdint.h>      // uint8_t, uint32_t
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "teclado.h"

#define FILAS 4
#define COLS  4

// Pines: 4 salidas (filas) y 4 entradas (columnas)
static const uint8_t pines_filas[FILAS] = {2, 3, 4, 5};
static const uint8_t pines_cols[COLS]   = {6, 7, 8, 9};

static const char mapa[FILAS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

void teclado_inicializar(void) {
    for (uint8_t i = 0; i < FILAS; i++) {
        gpio_init(pines_filas[i]);
        gpio_set_dir(pines_filas[i], GPIO_OUT);
        gpio_put(pines_filas[i], 1);          // filas en alto (inactivas)
    }
    for (uint8_t j = 0; j < COLS; j++) {
        gpio_init(pines_cols[j]);
        gpio_set_dir(pines_cols[j], GPIO_IN);
        gpio_pull_up(pines_cols[j]);          // columnas con pull-up interno
    }
}

static uint32_t ultima_presion = 0;           // para el anti-rebote

char teclado_leer(void) {
    uint32_t ahora = time_us_32();

    // Anti-rebote: ignoro lecturas demasiado juntas
    if (ahora - ultima_presion < 200000) {
        return 0;
    }

    for (uint8_t i = 0; i < FILAS; i++) {
        gpio_put(pines_filas[i], 0);          // activo una fila
        sleep_us(10);                          // espero a que se estabilice

        for (uint8_t j = 0; j < COLS; j++) {
            if (gpio_get(pines_cols[j]) == 0) {   // tecla presionada
                gpio_put(pines_filas[i], 1);
                ultima_presion = time_us_32();
                return mapa[i][j];
            }
        }
        gpio_put(pines_filas[i], 1);
    }
    return 0;                                  // sin tecla
}
