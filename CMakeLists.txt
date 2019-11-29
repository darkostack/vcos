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

set(CPU_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cpu/${CPU_NAME})

set(APPS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/apps)

set(CPU_INCLUDE_DIRECTORIES
    ${CPU_DIR}
)

if (${CPU_NAME} STREQUAL sirius)
    set(CPU_INCLUDE_DIRECTORIES
        ${CPU_INCLUDE_DIRECTORIES}
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/ARM/cmsis
    )
endif()

set(PROJECT_CONFIG_DEFINITIONS
    VCOS_PROJECT_CORE_CONFIG_FILE=\"vcos-core-${CPU_NAME}-config.h\"
)

add_library(vcos_src_core
    ${SRC_DIR}/core/api/assert_api.c
    ${SRC_DIR}/core/api/instance_api.cpp
    ${SRC_DIR}/core/api/msg_api.cpp
    ${SRC_DIR}/core/api/mutex_api.cpp
    ${SRC_DIR}/core/api/stdiobase_api.cpp
    ${SRC_DIR}/core/api/thread_api.cpp
    ${SRC_DIR}/core/api/timer_api.cpp
    ${SRC_DIR}/core/api/net/message_api.cpp
    ${SRC_DIR}/core/api/net/tasklet_api.cpp
    ${SRC_DIR}/core/common/instance.cpp
    ${SRC_DIR}/core/common/kernel.cpp
    ${SRC_DIR}/core/common/msg.cpp
    ${SRC_DIR}/core/common/mutex.cpp
    ${SRC_DIR}/core/common/thread.cpp
    ${SRC_DIR}/core/common/timer.cpp
    ${SRC_DIR}/core/common/random.cpp
    ${SRC_DIR}/core/utils/bitarithm.c
    ${SRC_DIR}/core/utils/isrpipe.cpp
    ${SRC_DIR}/core/utils/ringbuffer.cpp
    ${SRC_DIR}/core/utils/tinymt32/tinymt32.c
    ${SRC_DIR}/core/net/platform/alarm.c
    ${SRC_DIR}/core/net/platform.c
    ${SRC_DIR}/core/net/message.cpp
    ${SRC_DIR}/core/net/tasklet.cpp
    ${SRC_DIR}/core/net/timer.cpp
    ${SRC_DIR}/core/net/tlvs.cpp
    ${SRC_DIR}/core/net/trickle_timer.cpp
    ${SRC_DIR}/core/net/string.cpp
    ${SRC_DIR}/core/net/notifier.cpp
    ${SRC_DIR}/core/net/utils/heap.cpp
)

target_include_directories(vcos_src_core
    PRIVATE
        ${SRC_DIR}
        ${SRC_DIR}/core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CPU_INCLUDE_DIRECTORIES}
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
        ${CPU_INCLUDE_DIRECTORIES}
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
        ${CPU_INCLUDE_DIRECTORIES}
)

target_compile_options(vcos_src_arduino
    PRIVATE
        ${FIRST_PARTY_COMPILE_FLAGS}
)

add_executable(vcos_apps_cli
    ${APPS_DIR}/cli/main.c
)

target_link_libraries(vcos_apps_cli
    PRIVATE
        vcos_src_core
        vcos_src_cli
        vcos_cpu_${CPU_NAME}
)

add_executable(vcos_apps_arduino
    ${APPS_DIR}/arduino/main.cpp
)

target_link_libraries(vcos_apps_arduino
    PRIVATE
        vcos_src_core
        vcos_src_arduino
        vcos_cpu_${CPU_NAME}
)

if (${CPU_NAME} STREQUAL sirius)
    add_library(vcos_cpu_sirius
        ${CPU_DIR}/periph/gpio.c
        ${CPU_DIR}/periph/tim.c
        ${CPU_DIR}/periph/uart.c
        ${CPU_DIR}/cpu.c
        ${CPU_DIR}/irq.c
        ${CPU_DIR}/panic.c
        ${CPU_DIR}/syscalls.c
        ${CPU_DIR}/vectors.c
        ${CPU_DIR}/vectors_cortexm.c
    )

    target_include_directories(vcos_cpu_sirius
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CPU_INCLUDE_DIRECTORIES}
    )

    target_compile_options(vcos_cpu_sirius
        PRIVATE
            ${FIRST_PARTY_COMPILE_FLAGS}
    )

    #special link script
    set_target_properties(vcos_apps_cli PROPERTIES LINK_DEPENDS ${CPU_DIR}/sirius.ld)
    set_target_properties(vcos_apps_cli PROPERTIES LINK_FLAGS "-L${CMAKE_CURRENT_SOURCE_DIR}/build -Wl,--whole-archive -lvcos_src_core -lvcos_src_cli -lvcos_cpu_sirius -Wl,--no-whole-archive -T ${CPU_DIR}/sirius.ld -lc -lnosys -lm -lstdc++")

    set_target_properties(vcos_apps_arduino PROPERTIES LINK_DEPENDS ${CPU_DIR}/sirius.ld)
    set_target_properties(vcos_apps_arduino PROPERTIES LINK_FLAGS "-L${CMAKE_CURRENT_SOURCE_DIR}/build -Wl,--whole-archive -lvcos_src_core -lvcos_src_arduino -lvcos_cpu_sirius -Wl,--no-whole-archive -T ${CPU_DIR}/sirius.ld -lc -lnosys -lm -lstdc++")

    # build bin file
    add_custom_command(OUTPUT vcos_apps_cli.bin
        COMMAND arm-none-eabi-objcopy -O binary vcos_apps_cli vcos_apps_cli.bin
        DEPENDS vcos_apps_cli
    )
    add_custom_target(vcos_apps_cli_bin ALL DEPENDS vcos_apps_cli.bin)
    add_custom_target(vcos_apps_cli_size ALL
        COMMAND arm-none-eabi-size --format=berkeley vcos_apps_cli
        DEPENDS vcos_apps_cli
    )

    add_custom_command(OUTPUT vcos_apps_arduino.bin
        COMMAND arm-none-eabi-objcopy -O binary vcos_apps_arduino vcos_apps_arduino.bin
        DEPENDS vcos_apps_arduino
    )
    add_custom_target(vcos_apps_arduino_bin ALL DEPENDS vcos_apps_arduino.bin)
    add_custom_target(vcos_apps_arduino_size ALL
        COMMAND arm-none-eabi-size --format=berkeley vcos_apps_arduino
        DEPENDS vcos_apps_arduino
    )

endif()

set(PRETTY_DIRS
    ./src
    ./include
    ./cpu
    ./apps
)

add_custom_target(pretty
    find ${PRETTY_DIRS} \( -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -exec bash -c "echo Beautify '{}' && tools/script/clang-format.sh -i '{}'" \;
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
)
