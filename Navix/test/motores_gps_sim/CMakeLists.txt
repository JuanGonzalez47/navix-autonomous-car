# Nombre del ejecutable
add_executable(test_motores_gps_sim
    main.c 
    ../gps/gps.c
    ../motores/motores.c
    ../gy511/gy511.c
    ../bluetooth/bluetooth.c
)

# Configuración del ejecutable
pico_set_program_name(test_motores_gps_sim "test_motores_gps_sim")
pico_set_program_version(test_motores_gps_sim "1.0")

# Habilitar stdio (USB) para depuración
pico_enable_stdio_usb(test_motores_gps_sim 1)
pico_enable_stdio_uart(test_motores_gps_sim 0)

# Indicar rutas de los archivos .h (para que el compilador los encuentre)
target_include_directories(test_motores_gps_sim PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../gps
    ${CMAKE_CURRENT_LIST_DIR}/../motores
    ${CMAKE_CURRENT_LIST_DIR}/../gy511
    ${CMAKE_CURRENT_LIST_DIR}/../bluetooth
)

# Enlazar librerías necesarias
target_link_libraries(test_motores_gps_sim 
    pico_stdlib 
    hardware_pwm 
    hardware_uart 
    hardware_irq
    hardware_i2c      # <-- Necesario para la brújula GY-511
    pico_multicore    # <-- Necesario para arrancar el Core 1
    hardware_sync     # <-- Necesario para los spin_locks (memoria compartida)
)

# Generar archivos extra (.uf2, etc)
pico_add_extra_outputs(test_motores_gps_sim)