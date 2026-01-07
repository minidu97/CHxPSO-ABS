cmake_minimum_required(VERSION 3.15)
project(CHxPSO_ABS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include header files
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add executable
add_executable(chxpso_abs_run
    src/main.cpp
)
