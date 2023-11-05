add_executable(
    deserialization_test 
        src/deserialization_test.cpp
)

set_target_properties(deserialization_test PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

target_link_libraries(
    deserialization_test PRIVATE 
        ${PROJECT_NAME}
)

add_test(NAME deserialization_test COMMAND deserialization_test)
