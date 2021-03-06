cmake_minimum_required(VERSION 3.5.1)
project(crc-hamming-distance)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CUDA_SEPARABLE_COMPILATION ON)

include(CTest)
include(cmake/Catch.cmake)
find_package(CUDA 8.0)
find_package(OpenMP)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

file(GLOB_RECURSE lib_sources
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.cu"
)
set(main_source "${CMAKE_SOURCE_DIR}/src/main.cu")
list(REMOVE_ITEM lib_sources ${main_source})
file(GLOB_RECURSE test_sources
    "${CMAKE_SOURCE_DIR}/test/*.cpp"
    "${CMAKE_SOURCE_DIR}/test/*.cu"
)

# In the future, the library sources should be compiled separately
# with the two executables linking the library
CUDA_INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/include")
CUDA_ADD_EXECUTABLE(weights ${main_source} ${lib_sources})
CUDA_ADD_EXECUTABLE(tests ${test_sources} ${lib_sources})
target_include_directories(weights PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_include_directories(tests PUBLIC "${CMAKE_SOURCE_DIR}/include")

catch_discover_tests(tests)

add_definitions(-DUSE_CUDA)
include_directories(${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES}) 
