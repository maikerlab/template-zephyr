cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(unit_tests)

# Include paths:
#   ../../include  -- public headers (hal_iface.h)
#   ../../src      -- private headers (hal.h) when testing internal modules
#   .              -- mock_hal.h
target_include_directories(app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Test sources -- add new test files here.
target_sources(app PRIVATE
    src/smoke.c
)
