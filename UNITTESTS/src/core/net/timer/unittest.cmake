set(unittest-includes ${unittest-includes}
)

set(unittest-sources
    ../src/core/common/instance.cpp
    ../src/core/api/instance_api.cpp
    ../src/core/net/timer.cpp
)

set(unittest-test-sources
    src/core/net/timer/test_timer.cpp
    stubs/assert_api_stub.c
    stubs/timer_stub.cpp
    stubs/message_stub.cpp
    stubs/tasklet_stub.cpp
    stubs/platform_stub.cpp
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVCOS_PROJECT_CORE_CONFIG_FILE='\"vcos-core-unittest-config.h\"'")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVCOS_PROJECT_CORE_CONFIG_FILE='\"vcos-core-unittest-config.h\"'")
