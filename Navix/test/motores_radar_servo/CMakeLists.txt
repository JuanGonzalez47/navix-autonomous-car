# 1. Definición del ejecutable
add_executable(motores_radar_servo
    main_navix.c
    core1_logic.c
    ../radar/radar.c    # Asumiendo que tu driver de radar está en test/radar/
    ../servo/servo.c    # Asumiendo que tu driver de servo está en test/servo/
    ../motores/motores.c    # Asumiendo que tu driver de motor está en test/motores/
)

# 2. Configuración del programa
pico_set_program_name(motores_radar_servo "motores_radar_servo")
pico_set_program_version(motores_radar_servo "0.1")

# 3. Habilitar stdio
pico_enable_stdio_usb(motores_radar_servo 1)
pico_enable_stdio_uart(motores_radar_servo 0)

# 4. Incluir directorios para encontrar los archivos .h
# Es vital añadir cada carpeta donde vivan los headers (.h)
target_include_directories(motores_radar_servo PRIVATE
    .               # Carpeta actual
    ../radar        # Para encontrar radar.h
    ../servo        # Para encontrar servo.h
    ../motores        # Para encontrar motores.h
)

# 5. Enlazar librerías
target_link_libraries(motores_radar_servo
    pico_stdlib
    pico_multicore
    hardware_pwm
    hardware_timer
    hardware_irq
)

# 6. Generar archivos extra
pico_add_extra_outputs(motores_radar_servo) 