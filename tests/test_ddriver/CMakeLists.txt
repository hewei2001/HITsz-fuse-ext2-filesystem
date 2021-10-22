cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(ddriver_test VERSION 0.0.1 LANGUAGES C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_FILE_OFFSET_BITS=64")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall --pedantic -g")

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

include_directories(./include)
aux_source_directory(./src DIR_SRCS)
add_executable(ddriver_test ${DIR_SRCS})
target_link_libraries(ddriver_test $ENV{HOME}/lib/libddriver.a)
