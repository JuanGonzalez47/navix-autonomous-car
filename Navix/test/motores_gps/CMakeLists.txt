# Nombre del ejecutable
add_executable(test_motores_gps 
    main.c 
    ../gps/gps.c
    ../motores/motores.c
)

# Configuración del ejecutable
pico_set_program_name(test_motores_gps "test_motores_gps")
pico_set_program_version(test_motores_gps "0.1")

# Habilitar stdio (USB) para depuración
pico_enable_stdio_usb(test_motores_gps 1)
pico_enable_stdio_uart(test_motores_gps 0)

# Indicar rutas de los archivos .h (para que el compilador los encuentre)
target_include_directories(test_motores_gps PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../gps
    ${CMAKE_CURRENT_LIST_DIR}/../motores
)

# Enlazar librerías necesarias
target_link_libraries(test_motores_gps 
    pico_stdlib 
    hardware_pwm 
    hardware_uart 
    hardware_irq
)

# Generar archivos extra (.uf2, etc)
pico_add_extra_outputs(test_motores_gps)