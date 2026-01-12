cmake_minimum_required(VERSION 3.16)
project(CHxPSO_ABS LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler warnings
add_compile_options(-Wall -O3)

set(MAIN_SOURCES
    src/main.cpp
)

# ---- CEC 2013 ----
if(EXISTS "${CMAKE_SOURCE_DIR}/cec2013/test_func.cpp")
    message(STATUS "✓ CEC 2013 detected")
    list(APPEND MAIN_SOURCES cec2013/test_func.cpp)
    add_compile_definitions(USE_CEC2013)
    set(CEC2013_FOUND TRUE)
else()
    set(CEC2013_FOUND FALSE)
endif()

# ---- CEC 2017 ----
if(EXISTS "${CMAKE_SOURCE_DIR}/cec2017/cec17_test_func.c")
    message(STATUS "✓ CEC 2017 detected")
    list(APPEND MAIN_SOURCES
        cec2017/cec17_test_func.c
        cec2017/cec17.c
    )
    add_compile_definitions(USE_CEC2017)
    set(CEC2017_FOUND TRUE)
else()
    set(CEC2017_FOUND FALSE)
endif()

# ---- Executable ----
add_executable(chxpso ${MAIN_SOURCES})

# ---- Include paths (TARGET-BASED, CORRECT) ----
target_include_directories(chxpso PRIVATE
    ${CMAKE_SOURCE_DIR}/include   # for chxpso_abs.h, benchmarks.h, particle.h, abs_strategy.h
    ${CMAKE_SOURCE_DIR}/src       # for chclpso_abs.h, chppso_abs.h
    ${CMAKE_SOURCE_DIR}/cec2013   # CEC 2013
    ${CMAKE_SOURCE_DIR}/cec2017   # CEC 2017
)


# ---- Summary ----
message(STATUS "")
message(STATUS "========================================")
message(STATUS "CHxPSO-ABS Build Configuration")
message(STATUS "CEC 2013: ${CEC2013_FOUND}")
message(STATUS "CEC 2017: ${CEC2017_FOUND}")
message(STATUS "========================================")
