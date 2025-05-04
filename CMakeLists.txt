cmake_minimum_required(VERSION 3.18...3.29)
project(ssir VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Locate Python3
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Tell CMake about pybind11Config.cmake in .venv
set(pybind11_DIR "${CMAKE_SOURCE_DIR}/.venv/Lib/site-packages/pybind11/share/cmake/pybind11")
find_package(pybind11 CONFIG REQUIRED)

# Include headers and gather all cpp files
include_directories(${CMAKE_SOURCE_DIR}/include)
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

# Build the native executable
add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Build the Python extension module
pybind11_add_module(ssir-module bindings/ssir.cpp ${SOURCES})
target_include_directories(ssir-module PRIVATE ${CMAKE_SOURCE_DIR}/include)
