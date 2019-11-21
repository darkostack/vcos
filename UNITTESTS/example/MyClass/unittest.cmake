# Add here additional test specific include paths
set(unittest-includes ${unittest-includes}
    ../example
)

# Add here classes under test
set(unittest-sources
    ../example/MyClass.cpp
)

# Add here test classes and stubs
set(unittest-test-sources
    example/MyClass/test_MyClass.cpp
    stubs/OtherClass_stub.cpp
)
