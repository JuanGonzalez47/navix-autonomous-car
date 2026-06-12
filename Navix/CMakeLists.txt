# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

cmake_minimum_required(VERSION 3.13)

# Inicializar el SDK de Pico
include(pico_sdk_import.cmake)

project(Navix_Project C CXX ASM)
pico_sdk_init()

# Incluir tus subdirectorios
#add_subdirectory(test/motores)
#add_subdirectory(test/servo)
#add_subdirectory(test/radar)
#add_subdirectory(test/servo_radar)
#add_subdirectory(test/motores_radar_servo)
#add_subdirectory(test/gps)
#add_subdirectory(test/motores_gps)
#add_subdirectory(test/gy511)
add_subdirectory(test/bluetooth)