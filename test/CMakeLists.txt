cmake_minimum_required(VERSION 3.10)
project(out)
add_compile_options(-Wall)
add_subdirectory(../googletest gtest)
aux_source_directory(. SRC)
add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} gmock_main)
