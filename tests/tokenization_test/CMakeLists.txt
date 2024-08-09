# Tokenization test
add_executable(
    tokenization_test 
        src/main.cpp
)

target_compile_features(tokenization_test PUBLIC cxx_std_23)

target_link_libraries(
  tokenization_test PRIVATE 
        ${PROJECT_NAME}
)

add_test(NAME tokenization_test COMMAND tokenization_test)