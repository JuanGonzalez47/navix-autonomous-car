# Nombre del ejecutable
add_executable(test_servo_radar 
    main.c 
    ../radar/radar.c
    ../servo/servo.c
)

# Configuración del ejecutable
pico_set_program_name(test_servo_radar "test_servo_radar")
pico_set_program_version(test_servo_radar "0.1")

# Habilitar stdio (USB) para depuración
pico_enable_stdio_usb(test_servo_radar 1)
pico_enable_stdio_uart(test_servo_radar 0)

# Indicar rutas de los archivos .h (para que el compilador los encuentre)
target_include_directories(test_servo_radar PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/../radar
    ${CMAKE_CURRENT_LIST_DIR}/../servo
)

# Enlazar librerías necesarias
# Incluimos las que usas en ambos módulos (PWM, Timer, GPIO)
target_link_libraries(test_servo_radar 
    pico_stdlib 
    hardware_pwm 
    hardware_timer
    hardware_gpio
    hardware_sync
)

# Generar archivos extra (.uf2, etc)
pico_add_extra_outputs(test_servo_radar)