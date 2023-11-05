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

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)

enable_testing()
add_subdirectory(tests)