cmake_minimum_required(VERSION 3.31.3)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

project(adams_libs C)

add_subdirectory(adm_utils)
add_subdirectory(adm_framework)
