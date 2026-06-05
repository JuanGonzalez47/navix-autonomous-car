add_executable(test_servo 
    main.c 
    servo.c
)

pico_enable_stdio_usb(test_servo 1)
pico_enable_stdio_uart(test_servo 0)

target_link_libraries(test_servo 
    pico_stdlib 
    hardware_pwm
    hardware_timer 
)

pico_add_extra_outputs(test_servo)