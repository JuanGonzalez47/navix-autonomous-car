add_executable(test_bluetooth 
    main.c 
    bluetooth.c
)

pico_set_program_name(test_bluetooth "test_bluetooth")
pico_set_program_version(test_bluetooth "0.1")

pico_enable_stdio_usb(test_bluetooth 1)
pico_enable_stdio_uart(test_bluetooth 0)

target_link_libraries(test_bluetooth 
    pico_stdlib 
    hardware_uart
)

pico_add_extra_outputs(test_bluetooth)