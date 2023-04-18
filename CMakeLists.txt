cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel (CMake defaults)")

project(test-mpi CXX)

# set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)    # export all symbols from dll on Windows
# set(CMAKE_ENABLE_EXPORTS ON)                # export all symbols from exe on Windows

# if(MSVC)
#   add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -D_WIN32_WINNT=0x0601)
# endif()

find_package(MPI REQUIRED)

add_executable(test-mpi test_mpi.cpp)

target_link_libraries(test-mpi MPI::MPI_CXX)
