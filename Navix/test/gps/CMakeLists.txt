# Nombre del ejecutable para esta prueba unitaria
add_executable(test_gps 
    main.c 
    gps.c
)

# Configuración del ejecutable
pico_set_program_name(test_gps "test_gps")
pico_set_program_version(test_gps "0.1")

# Habilitar stdio (USB) para depuración
pico_enable_stdio_usb(test_gps 1)
pico_enable_stdio_uart(test_gps 0)

# Enlazar librerías necesarias (UART e IRQ son vitales aquí)
target_link_libraries(test_gps 
    pico_stdlib 
    hardware_uart
    hardware_irq
)

# Generar archivos extra (.uf2, etc)
pico_add_extra_outputs(test_gps)