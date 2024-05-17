cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(rc_motor)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_executable(rc_motor
    src/rc_motor
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(rc_motor pico_stdlib pico_cyw43_arch_none)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(rc_motor)
# example_auto_set_url(hello_world)