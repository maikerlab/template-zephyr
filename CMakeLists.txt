cmake_minimum_required(VERSION 3.20.0)

# Zephyr must be found before project() -- it sets up the toolchain.
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(maikerlab_zephyr_template)

# Public headers (hal_iface.h)
target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Application sources -- add new .c files here.
target_sources(app PRIVATE
    src/main.c
    src/hal.c
)
