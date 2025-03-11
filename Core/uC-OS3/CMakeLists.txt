add_library(uC-OS3 STATIC)

target_sources(uC-OS3 PRIVATE
    # CPU
    ${CMAKE_CURRENT_SOURCE_DIR}/CPU/cpu_a.s
    ${CMAKE_CURRENT_SOURCE_DIR}/CPU/cpu_c.c
    ${CMAKE_CURRENT_SOURCE_DIR}/CPU/cpu_core.c

    # ARM Cortex-M Port
    ${CMAKE_CURRENT_SOURCE_DIR}/Ports/ARM-Cortex-M/ARMv7-M/os_cpu_a.S
    ${CMAKE_CURRENT_SOURCE_DIR}/Ports/ARM-Cortex-M/ARMv7-M/os_cpu_c.c

    # Source files
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/os_app_hooks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/os_cfg_app.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/os_core.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/os_dbg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/os_flag.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/os_mem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/os_msg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/os_mutex.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/os_prio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/os_q.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/os_sem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/os_stat.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/os_task.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/os_tick.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/os_time.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/os_tmr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/os_var.c
)

target_include_directories(uC-OS3 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/CPU
    ${CMAKE_CURRENT_SOURCE_DIR}/Ports/ARM-Cortex-M/ARMv7-M
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)
