cmake_minimum_required(VERSION 3.20)

project(space_invaders
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ----
# dependencies
# ----
include(FetchContent)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 12.2.0
)
FetchContent_MakeAvailable(fmt)

# -----
# intel 8080
# -----
add_library(i8080
    src/i8080/cpu.cpp
    src/i8080/bus.cpp
    src/i8080/disasm.cpp

)

target_include_directories(i8080
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(i8080
    PUBLIC
        fmt::fmt
)

# -----
# emulator
# -----
add_executable(space_invaders
    src/emu/main.cpp
)


target_include_directories(space_invaders
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(space_invaders
    PRIVATE
        i8080
)

# -----
# 8080 disassembler
# ----

add_executable(i8080_disasm
    src/disasm/main.cpp 
)

target_include_directories(i8080_disasm
    PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(i8080_disasm
    PRIVATE
        i8080
)

# -----
# Tests
# -----

include(CTest)

if(BUILD_TESTING)
    include(FetchContent)


    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.18.0
    )

    FetchContent_MakeAvailable(googletest)

    add_executable(i8080_tests
        tests/cpu_test.cpp
        tests/bus_test.cpp
    )

    target_include_directories(i8080_tests
        PRIVATE
        ${PROJECT_SOURCE_DIR}/include
    )

    target_link_libraries(i8080_tests
        PRIVATE
            i8080
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(i8080_tests)
endif()
