cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(i2c_io)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_executable(i2c_io
    src/i2c_io.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(i2c_io pico_stdlib)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(i2c_io)
# example_auto_set_url(hello_world)