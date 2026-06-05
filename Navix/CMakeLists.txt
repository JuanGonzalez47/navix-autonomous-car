cmake_minimum_required(VERSION 3.13)

# Inicializar el SDK de Pico
include(pico_sdk_import.cmake)

project(Navix_Project C CXX ASM)
pico_sdk_init()

# Incluir tus subdirectorios
add_subdirectory(test/motores)
add_subdirectory(test/servo)
add_subdirectory(test/radar)
add_subdirectory(test/servo_radar)