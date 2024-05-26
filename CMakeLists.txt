cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(motor_control)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_executable(motor_control
    src/motor_control.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(motor_control pico_stdlib hardware_pwm pico_stdio_usb)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(motor_control)
# example_auto_set_url(hello_world)