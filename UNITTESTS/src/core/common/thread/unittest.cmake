set(unittest-includes ${unittest-includes}
)

set(unittest-sources
    ../src/core/common/thread.cpp
)

set(unittest-test-sources
    src/core/common/thread/test_thread.cpp
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVCOS_PROJECT_CORE_CONFIG_FILE='\"vcos-core-unittest-config.h\"'")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVCOS_PROJECT_CORE_CONFIG_FILE='\"vcos-core-unittest-config.h\"'")
