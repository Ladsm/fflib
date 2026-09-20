cmake_minimum_required(VERSION 3.10)
project(fflib_test VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SOURCES 
    src/main.cpp
)
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
if(WIN32)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
    target_link_libraries(${PROJECT_NAME} PRIVATE 
        User32.lib 
        Gdi32.lib
        Shell32.lib
        Advapi32.lib
    )
else()
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
endif()