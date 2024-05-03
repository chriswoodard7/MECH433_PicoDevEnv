cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(pico_zero)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_executable(pico_zero
    src/pico_zero.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(pico_zero pico_stdlib hardware_adc hardware_irq hardware_uart pico_binary_info)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(pico_zero)

pico_enable_stdio_usb(pico_zero 1)
pico_enable_stdio_uart(pico_zero 0)
# example_auto_set_url(hello_world)