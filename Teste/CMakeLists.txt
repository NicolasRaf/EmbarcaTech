# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(Teste C CXX ASM)
set(RED_LED_PIN 13)
set(GREEN_LED_PIN 11)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(Teste 
    ./src/Teste.c
    ./src/server.c
    ./src/wifi_manager.c)

pico_set_program_name(Teste "Teste")
pico_set_program_version(Teste "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(Teste 1)
pico_enable_stdio_usb(Teste 1)

# Add the standard library to the build
target_link_libraries(Teste
    pico_stdlib
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip
)

# Add the standard include files to the build
target_include_directories(Teste PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_SOURCE_DIR}/inc
)

target_compile_definitions(Teste PRIVATE
    PICO_LWIP_IPV4_ENABLED
    PICO_LWIP_TCP_ENABLED
    PICO_LWIP_UDP_ENABLED
    RED_LED_PIN=${RED_LED_PIN}
    GREEN_LED_PIN=${GREEN_LED_PIN}
)

pico_add_extra_outputs(Teste)
