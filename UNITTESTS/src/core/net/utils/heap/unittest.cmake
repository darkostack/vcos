set(unittest-includes ${unittest-includes}
)

set(unittest-sources
    ../src/core/net/utils/heap.cpp
)

set(unittest-test-sources
    src/core/net/utils/heap/test_heap.cpp
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVCOS_PROJECT_CORE_CONFIG_FILE='\"vcos-core-unittest-config.h\"'")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVCOS_PROJECT_CORE_CONFIG_FILE='\"vcos-core-unittest-config.h\"'")
