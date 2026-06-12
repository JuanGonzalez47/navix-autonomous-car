add_executable(test_navegacion_base 
    main.c 
    ../gps/gps.c
    ../gy511/gy511.c
    ../bluetooth/bluetooth.c
    ../motores/motores.c
)

pico_set_program_name(test_navegacion_base "test_nav")
pico_set_program_version(test_navegacion_base "0.1")

pico_enable_stdio_usb(test_navegacion_base 1)
pico_enable_stdio_uart(test_navegacion_base 0)

# Le decimos dónde encontrar los archivos .h
target_include_directories(test_navegacion_base PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../gps
    ${CMAKE_CURRENT_LIST_DIR}/../gy511
    ${CMAKE_CURRENT_LIST_DIR}/../bluetooth
    ${CMAKE_CURRENT_LIST_DIR}/../motores
)

# Enlazamos todas las herramientas de hardware
target_link_libraries(test_navegacion_base 
    pico_stdlib 
    hardware_uart
    hardware_i2c
    hardware_pwm
    hardware_irq
    pico_multicore 
    hardware_sync
)

pico_add_extra_outputs(test_navegacion_base)