cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(RASPI2_G6 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(RASPI2_G6
    Sources/RASPI2_G6.c
    Devices/I2C_lcd.c
    Devices/teclado.c
    Devices/uart_comm.c
    Drivers/uart_driver.c
)

target_include_directories(RASPI2_G6 PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/Devices
    ${CMAKE_CURRENT_LIST_DIR}/Drivers
    ${CMAKE_CURRENT_LIST_DIR}/Sources
)

target_link_libraries(RASPI2_G6
    pico_stdlib
    hardware_i2c
    hardware_uart
    hardware_irq
    hardware_resets
    hardware_timer
)

# stdout por USB para la consola serie
pico_enable_stdio_usb(RASPI2_G6 1)
pico_enable_stdio_uart(RASPI2_G6 0)

pico_add_extra_outputs(RASPI2_G6)
