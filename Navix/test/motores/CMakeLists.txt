# Nombre del ejecutable para esta prueba
add_executable(test_motores 
    main.c 
    motores.c
)

# Configuración del ejecutable
pico_set_program_name(test_motores "test_motores")
pico_set_program_version(test_motores "0.1")

# Habilitar stdio (USB) para depuración
pico_enable_stdio_usb(test_motores 1)
pico_enable_stdio_uart(test_motores 0)

# Enlazar librerías necesarias
target_link_libraries(test_motores 
    pico_stdlib 
    hardware_pwm 
    hardware_timer
)

# Generar archivos extra (.uf2, etc)
pico_add_extra_outputs(test_motores)