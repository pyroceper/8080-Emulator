cmake_minimum_required(VERSION 3.20)

project(space_invaders
    VERSION 0.1.0
    LANGUAGES CXX
)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_executable(space_invaders
    src/main.cpp
)


target_include_directories(space_invaders
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)
