cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(spi_dac)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_executable(spi_dac
    src/spi_dac.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(spi_dac pico_stdlib hardware_spi pico_binary_info)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(spi_dac)
# example_auto_set_url(hello_world)