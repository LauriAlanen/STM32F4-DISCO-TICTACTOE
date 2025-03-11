add_library(stm32-bsp STATIC)

target_sources(stm32-bsp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32f429i_discovery.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32f429i_discovery_eeprom.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32f429i_discovery_gyroscope.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32f429i_discovery_io.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32f429i_discovery_lcd.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32f429i_discovery_sdram.c
    ${CMAKE_CURRENT_SOURCE_DIR}/stm32f429i_discovery_ts.c
)

target_include_directories(stm32-bsp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Drivers/STM32F4xx_HAL_Driver/Inc
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Include
)

target_link_libraries(stm32-bsp PUBLIC stm32cubemx uC-OS3)

