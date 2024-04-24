cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(cdc_io_adc)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_executable(cdc_io_adc
    src/cdc_io_adc
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(cdc_io_adc pico_stdlib hardware_adc)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(cdc_io_adc)
# example_auto_set_url(hello_world)

pico_enable_stdio_usb(cdc_io_adc 1) 
pico_enable_stdio_uart(cdc_io_adc 0)