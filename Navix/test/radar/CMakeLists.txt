# Nombre del ejecutable para esta prueba aislada
add_executable(test_radar 
    main.c 
    radar.c
)

# Configuración del ejecutable
pico_set_program_name(test_radar "test_radar")
pico_set_program_version(test_radar "0.1")

# Habilitar stdio (USB) para depuración
pico_enable_stdio_usb(test_radar 1)
pico_enable_stdio_uart(test_radar 0)

# Enlazar librerías necesarias
target_link_libraries(test_radar 
    pico_stdlib 
    hardware_timer
    hardware_irq
)

# Generar archivos extra (.uf2, etc)
pico_add_extra_outputs(test_radar)