set(unittest-includes ${unittest-includes}
)

set(unittest-sources
    ../src/core/utils/isrpipe.cpp
)

set(unittest-test-sources
    src/core/utils/isrpipe/test_isrpipe_tsrb.cpp
    stubs/assert_api_stub.c
    stubs/mutex_stub.cpp
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVCOS_PROJECT_CORE_CONFIG_FILE='\"vcos-core-unittest-config.h\"'")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVCOS_PROJECT_CORE_CONFIG_FILE='\"vcos-core-unittest-config.h\"'")
