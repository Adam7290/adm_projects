cmake_minimum_required(VERSION 3.31.3)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_STANDARD 17)

project(adams_libs C)

add_subdirectory(src/adm_utils)
add_subdirectory(src/adm_framework)