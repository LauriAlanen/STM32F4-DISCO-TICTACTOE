aux_source_directory(Source UCOS_SRC)
aux_source_directory(Ports/ARM-Cortex-M/ARMv7-M UCOS_PORT_SRC)
aux_source_directory(CPU UCOS_CPU)

add_library(uC-OS3 STATIC ${UCOS_SRC} ${UCOS_PORT_SRC} ${UCOS_CPU})

target_include_directories(uC-OS3 PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/Ports/ARM-Cortex-M/ARMv7-M
    ${CMAKE_CURRENT_SOURCE_DIR}/CPU
)
