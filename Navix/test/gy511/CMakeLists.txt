add_executable(test_gy511 
    main.c 
    gy511.c
)

pico_set_program_name(test_gy511 "test_gy511")
pico_set_program_version(test_gy511 "0.1")

pico_enable_stdio_usb(test_gy511 1)
pico_enable_stdio_uart(test_gy511 0)

# Importante: Aquí enlazamos hardware_i2c en lugar de hardware_uart
target_link_libraries(test_gy511 
    pico_stdlib 
    hardware_i2c
)

pico_add_extra_outputs(test_gy511)