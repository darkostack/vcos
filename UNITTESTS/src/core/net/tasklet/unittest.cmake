set(unittest-includes ${unittest-includes}
)

set(unittest-sources
    ../src/core/common/instance.cpp
    ../src/core/net/tasklet.cpp
)

set(unittest-test-sources
    src/core/net/tasklet/test_tasklet.cpp
    stubs/assert_api_stub.c
    stubs/tasklet_api_stub.cpp
    stubs/timer_stub.cpp
    stubs/message_stub.cpp
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVCOS_PROJECT_CORE_CONFIG_FILE='\"vcos-core-unittest-config.h\"'")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVCOS_PROJECT_CORE_CONFIG_FILE='\"vcos-core-unittest-config.h\"'")
