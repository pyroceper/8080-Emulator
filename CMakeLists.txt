cmake_minimum_required(VERSION 3.20)

project(space_invaders
    VERSION 0.1.0
    LANGUAGES CXX
)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -----
# emulator
# -----
add_executable(space_invaders
    src/emu/main.cpp

    src/i8080/cpu.cpp
)


target_include_directories(space_invaders
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

# -----
# 8080 disassembler
# ----

add_executable(i8080_disasm
    src/disasm/main.cpp 

    src/i8080/cpu.cpp
)

target_include_directories(i8080_disasm
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
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
        src/i8080/cpu.cpp
    )

    target_include_directories(i8080_tests
        PRIVATE
        ${PROJECT_SOURCE_DIR}/include
    )

    target_link_libraries(i8080_tests
        PRIVATE
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(i8080_tests)
endif()
