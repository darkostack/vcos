set(unittest-includes ${unittest-includes}
)

set(unittest-sources
    ../src/core/utils/bitarithm.c
)

set(unittest-test-sources
    src/core/utils/bitarithm/test_bitarithm.cpp
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVCOS_PROJECT_CORE_CONFIG_FILE='\"vcos-core-unittest-config.h\"'")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVCOS_PROJECT_CORE_CONFIG_FILE='\"vcos-core-unittest-config.h\"'")
