add_executable(
    tokenization_test 
        src/tokenization_test.cpp
)

set_target_properties(tokenization_test PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

target_link_libraries(
  tokenization_test PRIVATE 
        ${PROJECT_NAME}
)

add_test(NAME tokenization_test COMMAND tokenization_test)