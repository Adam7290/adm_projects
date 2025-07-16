cmake_minimum_required(VERSION 3.31.3)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_STANDARD 17)

set(LIBRARY_NAME adm_utils)
set(LIBRARY_NAME_TEST adm_utils_test_runner)
set(LIBRARY_NAME_SANDBOX adm_utils_sandbox)

project(${LIBRARY_NAME} C)

# Lib
add_library(${LIBRARY_NAME} 
    arena.c 
    panic.c
    string.c 
    hash.c
    iter.c
    format.c
    stream.c
    console.c
)
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Add math
find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
    target_link_libraries(${LIBRARY_NAME} PUBLIC ${MATH_LIBRARY})
endif()

# Tests
add_executable(${LIBRARY_NAME_TEST} 
    tests/framework/test.c 
    tests/test_main.c 
    tests/test_arena.c 
    tests/test_vector.c 
    tests/test_list.c
    tests/test_string.c 
    tests/test_test.c
    tests/test_iter.c
    tests/test_map.c
	tests/test_format.c
)
target_link_libraries(${LIBRARY_NAME_TEST} PRIVATE ${LIBRARY_NAME})

# Sandbox
add_executable(${LIBRARY_NAME_SANDBOX} 
    sandbox/main.c
)
target_link_libraries(${LIBRARY_NAME_SANDBOX} PRIVATE ${LIBRARY_NAME})
