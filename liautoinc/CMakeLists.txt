cmake_minimum_required(VERSION 3.0)
project(MyProject)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17")

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(INCLUDE_DIR ${ROOT_DIR}/include)
set(LIB_DIR ${ROOT_DIR}/lib)

set(CMAKE_FIND_ROOT_PATH "${CMAKE_FIND_ROOT_PATH}; ${ANDROID_NDK};${ROOT_DIR}")
message("CMAKE_FIND_ROOT_PATH: ${CMAKE_FIND_ROOT_PATH}")

find_library(my-lib liautoinc)
find_library(log-lib log)
find_library(dl-lib dl)

set(LIBS ${log-lib} ${my-lib} ${dl-lib})
message("${LIBS}")

add_executable(main main.cpp)
target_include_directories(main PRIVATE ${INCLUDE_DIR})
target_link_libraries(main PRIVATE ${LIBS})
