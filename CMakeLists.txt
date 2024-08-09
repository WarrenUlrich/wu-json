cmake_minimum_required(VERSION 3.15)

project(wu-json)

add_library(
    ${PROJECT_NAME} INTERFACE
)

target_include_directories(${PROJECT_NAME}
    INTERFACE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_23)

enable_testing()

add_subdirectory(tests/tokenization_test)