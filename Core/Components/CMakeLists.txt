cmake_minimum_required(VERSION 3.10)

# Get all source files recursively
file(GLOB_RECURSE COMPONENT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

# Get all header files recursively (for IDE support)
file(GLOB_RECURSE COMPONENT_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# Create a static library for the components
add_library(components STATIC ${COMPONENT_SOURCES} ${COMPONENT_HEADERS})

# Include directories for all components
target_include_directories(components PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Link dependencies if needed (modify accordingly)
target_link_libraries(components PUBLIC stm32-bsp)
