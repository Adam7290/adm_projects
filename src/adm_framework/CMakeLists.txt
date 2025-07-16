cmake_minimum_required(VERSION 3.31.3)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_STANDARD 17)

set(LIBRARY_NAME adm_framework)
set(LIBRARY_NAME_SANDBOX adm_framework_sandbox)

project(${LIBRARY_NAME} C)

add_library(${LIBRARY_NAME} app.c input.c gpu.c time.c image.c)
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Add adm_utils
target_link_libraries(${LIBRARY_NAME} PUBLIC adm_utils)

# Add GLFW
add_subdirectory(external/glfw)
target_link_libraries(${LIBRARY_NAME} PUBLIC glfw)

# Add GLAD
add_subdirectory(external/glad)
target_link_libraries(${LIBRARY_NAME} PUBLIC glad)

add_executable(${LIBRARY_NAME_SANDBOX} sandbox/main.c)
target_link_libraries(${LIBRARY_NAME_SANDBOX} PRIVATE ${LIBRARY_NAME})
