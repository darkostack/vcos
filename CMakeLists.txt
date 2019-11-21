cmake_minimum_required (VERSION 3.7)

project(vertexcom-os)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-Os")
set(CMAKE_C_FLAGS_RELEASE "-Os")

set(FIRST_PARTY_COMPILE_FLAGS
    -Wall
    -Wextra
    -Wshadow
    -Werror
    $<$<COMPILE_LANGUAGE:CXX>:
       -std=gnu++14
       -fno-exceptions
       -fno-rtti
    >
)

set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(PLATFORM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/platform/${PLATFORM_NAME})

set(APPS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/apps)

set(PLATFORM_INCLUDE_DIRECTORIES
    ${PLATFORM_DIR}
)

if (${PLATFORM_NAME} STREQUAL sirius)
    set(PLATFORM_INCLUDE_DIRECTORIES
        ${PLATFORM_INCLUDE_DIRECTORIES}
        ${PLATFORM_DIR}/arduino
        ${PLATFORM_DIR}/cmsis
    )
endif()

set(PROJECT_CONFIG_DEFINITIONS
    VCOS_PROJECT_CORE_CONFIG_FILE=\"vcos-core-${PLATFORM_NAME}-config.h\"
)

add_library(vcos_src_core
    ${SRC_DIR}/core/api/assert_api.c
    ${SRC_DIR}/core/api/instance_api.cpp
    ${SRC_DIR}/core/api/msg_api.cpp
    ${SRC_DIR}/core/api/mutex_api.cpp
    ${SRC_DIR}/core/api/stdiobase_api.cpp
    ${SRC_DIR}/core/api/thread_api.cpp
    ${SRC_DIR}/core/api/timer_api.cpp
    ${SRC_DIR}/core/common/instance.cpp
    ${SRC_DIR}/core/common/kernel_init.cpp
    ${SRC_DIR}/core/common/msg.cpp
    ${SRC_DIR}/core/common/mutex.cpp
    ${SRC_DIR}/core/common/thread.cpp
    ${SRC_DIR}/core/common/timer.cpp
    ${SRC_DIR}/core/utils/bitarithm.c
    ${SRC_DIR}/core/utils/isrpipe.cpp
    ${SRC_DIR}/core/utils/ringbuffer.cpp
)

target_include_directories(vcos_src_core
    PRIVATE
        ${SRC_DIR}
        ${SRC_DIR}/core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${PLATFORM_INCLUDE_DIRECTORIES}
)

target_compile_options(vcos_src_core
    PRIVATE
        ${FIRST_PARTY_COMPILE_FLAGS}
)

target_compile_definitions(vcos_src_core
    PUBLIC
        ${PROJECT_CONFIG_DEFINITIONS}
)

add_library(vcos_src_cli
    ${SRC_DIR}/cli/cli.cpp
    ${SRC_DIR}/cli/cli_uart.cpp
)

target_include_directories(vcos_src_cli
    PRIVATE
        ${SRC_DIR}
        ${SRC_DIR}/core
        ${SRC_DIR}/cli
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${PLATFORM_INCLUDE_DIRECTORIES}
)

target_compile_options(vcos_src_cli
    PRIVATE
        ${FIRST_PARTY_COMPILE_FLAGS}
)

target_compile_definitions(vcos_src_cli
    PUBLIC
        ${PROJECT_CONFIG_DEFINITIONS}
)

add_library(vcos_src_arduino
    ${SRC_DIR}/arduino/base.cpp
    ${SRC_DIR}/arduino/serialport.cpp
)

target_include_directories(vcos_src_arduino
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${PLATFORM_INCLUDE_DIRECTORIES}
)

target_compile_options(vcos_src_arduino
    PRIVATE
        ${FIRST_PARTY_COMPILE_FLAGS}
)

add_executable(vcos_apps_cli
    ${APPS_DIR}/cli/main.c
)

target_link_libraries(vcos_apps_cli
    PUBLIC
        vcos_src_core
        vcos_src_cli
        vcos_platform_${PLATFORM_NAME}
)

add_executable(vcos_apps_arduino
    ${APPS_DIR}/arduino/main.cpp
)

target_link_libraries(vcos_apps_arduino
    PUBLIC
        vcos_src_core
        vcos_src_arduino
        vcos_platform_${PLATFORM_NAME}
)

if (${PLATFORM_NAME} STREQUAL sirius)
    add_library(vcos_platform_sirius
        ${PLATFORM_DIR}/periph/gpio.c
        ${PLATFORM_DIR}/periph/tim.c
        ${PLATFORM_DIR}/periph/uart.c
        ${PLATFORM_DIR}/cpu_init.c
        ${PLATFORM_DIR}/irq.c
        ${PLATFORM_DIR}/panic.c
        ${PLATFORM_DIR}/syscalls.c
        ${PLATFORM_DIR}/vectors.c
        ${PLATFORM_DIR}/vectors_cortexm.c
    )

    target_include_directories(vcos_platform_sirius
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${PLATFORM_INCLUDE_DIRECTORIES}
    )

    target_compile_options(vcos_platform_sirius
        PRIVATE
            ${FIRST_PARTY_COMPILE_FLAGS}
    )

    #special link script
    set_target_properties(vcos_apps_cli PROPERTIES LINK_DEPENDS ${PLATFORM_DIR}/sirius.ld)
    set_target_properties(vcos_apps_cli PROPERTIES LINK_FLAGS "-L${CMAKE_CURRENT_SOURCE_DIR}/build -Wl,--whole-archive -lvcos_src_core -lvcos_src_cli -lvcos_platform_sirius -Wl,--no-whole-archive -T ${PLATFORM_DIR}/sirius.ld -lc -lnosys -lm -lstdc++")

    set_target_properties(vcos_apps_arduino PROPERTIES LINK_DEPENDS ${PLATFORM_DIR}/sirius.ld)
    set_target_properties(vcos_apps_arduino PROPERTIES LINK_FLAGS "-L${CMAKE_CURRENT_SOURCE_DIR}/build -Wl,--whole-archive -lvcos_src_core -lvcos_src_arduino -lvcos_platform_sirius -Wl,--no-whole-archive -T ${PLATFORM_DIR}/sirius.ld -lc -lnosys -lm -lstdc++")

    # build bin file
    add_custom_command(OUTPUT vcos_apps_cli.bin
        COMMAND arm-none-eabi-objcopy -O binary vcos_apps_cli vcos_apps_cli.bin
        DEPENDS vcos_apps_cli
    )
    add_custom_target(vcos_apps_cli_bin ALL DEPENDS vcos_apps_cli.bin)

    add_custom_command(OUTPUT vcos_apps_arduino.bin
        COMMAND arm-none-eabi-objcopy -O binary vcos_apps_arduino vcos_apps_arduino.bin
        DEPENDS vcos_apps_arduino
    )
    add_custom_target(vcos_apps_arduino_bin ALL DEPENDS vcos_apps_arduino.bin)

endif()
