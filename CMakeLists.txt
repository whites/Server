cmake_minimum_required(VERSION 3.0.0)
project(Server VERSION 0.1.0)

include(CTest)
enable_testing()

set(CMAKE_CXX_FLAGS   "-g")

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

aux_source_directory(. src_path)

add_executable(Server ${src_path})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
