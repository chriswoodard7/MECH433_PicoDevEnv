cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(imu_mouse)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_executable(imu_mouse
    src/imu_mouse.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(imu_mouse pico_stdlib pico_binary_info hardware_i2c pico_stdio_usb)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(imu_mouse)
# example_auto_set_url(hello_world)